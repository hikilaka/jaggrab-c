#include "server.h"

#include <stdio.h>
#include <ev.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <string.h>
#include "memory.h"
#include "file_system.h"

// need to make a idle timeout

static struct ev_loop* loop;
static struct ev_io accept_watcher;

static void handle_accept(struct ev_loop* loop, struct ev_io* watcher, int flags);
static void handle_read(struct ev_loop* loop, struct ev_io* watcher, int flags);
static void handle_timeout(struct ev_loop* loop, struct ev_timer* watcher, int flags);

int server_init(uint16_t port)
{
	loop = ev_default_loop(EVFLAG_AUTO);
	struct sockaddr_in addr;
	size_t addr_sz = sizeof(struct sockaddr_in);
	int fd;

	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		return 1;
	}
	memset(&addr, 0, addr_sz);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &addr, addr_sz);
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &addr, addr_sz);

	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	if (bind(fd, (struct sockaddr*) &addr, addr_sz) != 0)
	{
		perror("bind error");
		return 1;
	}
	if (listen(fd, 2) < 0)
	{
		perror("listen error");
		return 1;
	}
	ev_io_init(&accept_watcher, handle_accept, fd, EV_READ);
	ev_io_start(loop, &accept_watcher);
	return 0;
}

void server_loop(void)
{
	ev_loop(loop, 0);
}

static void close_conn(struct ev_loop* loop, struct ev_io* watcher)
{
	ev_io_stop(loop, watcher);
	close(watcher->fd);
	//free(watcher);
}

void handle_accept(struct ev_loop* loop, struct ev_io* watcher, int flags)
{
	if (EV_ERROR & flags)
	{
		perror("invalid event in handle_accept");
		return;
	}

	struct sockaddr_in addr;
	socklen_t addr_sz = sizeof(struct sockaddr_in);
	int fd = accept(watcher->fd, (struct sockaddr*) &addr, &addr_sz);
	if (fd < 0)
	{
		perror("accept error");
		return;
	}
	int fd_flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, fd_flags | O_NONBLOCK);

	struct ev_io* client = NULL;// = safe_alloc(sizeof(struct ev_io));
	ev_io_init(client, handle_read, fd, EV_READ);
	ev_io_start(loop, client);
}

void handle_read(struct ev_loop* loop, struct ev_io* watcher, int flags)
{
	if (EV_ERROR & flags)
	{
		perror("invalid event in handle_read");
		close_conn(loop, watcher);
		return;
	}
	char data[129];
	memset(data, 0, 129);
	ssize_t read = recv(watcher->fd, data, 128, 0);

	if (read < 0)
	{
		perror("read error");
		close_conn(loop, watcher);
		return;
	}
	if (read == 0)
	{
		ev_io_stop(loop, watcher);
		free(watcher);
		printf("client diconnected\n");
		return;
	}
	char* jag_ptr = strstr(data, "JAGGRAB /");
	char* line_ptr = strstr(data, "\n\n");

	if (jag_ptr != NULL && line_ptr != NULL)
	{
		jag_ptr += 9;
		size_t len = line_ptr - jag_ptr;
		char req[len + 1];
		memcpy(req, jag_ptr, len);
		req[len] = '\0';

		printf("JAGGRAB /%s\t%d\n", req, watcher->fd);
		buffer_t* buff = NULL;

		if (!strncmp("crc", req, 3))
			buff = file_sys_get_crc_tbl();
		else if (!strncmp("title", req, 5))
			buff = file_sys_get_file(0, 1);
		else if (!strncmp("config", req, 6))
			buff = file_sys_get_file(0, 2);
		else if (!strncmp("interface", req, 9))
			buff = file_sys_get_file(0, 3);
		else if (!strncmp("media", req, 5))
			buff = file_sys_get_file(0, 4);
		else if (!strncmp("versionlist", req, 11))
			buff = file_sys_get_file(0, 5);
		else if (!strncmp("textures", req, 8))
			buff = file_sys_get_file(0, 6);
		else if (!strncmp("wordenc", req, 7))
			buff = file_sys_get_file(0, 7);
		else if (!strncmp("sounds", req, 6))
			buff = file_sys_get_file(0, 8);
		else
			printf("unknown req file: %s\n", req);

		if (buff != NULL)
		{
			size_t blocks = buff->size / 512;
			ssize_t sent = 0;

			if ((buff->size % 512) != 0)
				blocks++;
			
			for (size_t i = 0; i < blocks; ++i)
			{
				size_t send_sz = buff->size - 512;
				if (send_sz > 512)
					send_sz = 512;

				ssize_t tmp = send(watcher->fd, buff->payload + sent, send_sz, 0);
				if (tmp == -1)
				{
					perror("send error");
					break;
				}
				sent += tmp;
			}
			free_buffer(buff);
		}
	}
	close_conn(loop, watcher);
}

void handle_timeout(struct ev_loop* loop, struct ev_timer* watcher, int flags)
{
	//close_conn(loop,);
}