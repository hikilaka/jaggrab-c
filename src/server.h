#ifndef SERVER_H
#define SERVER_H

#pragma once

#include <stdint.h>

int server_init(uint16_t port);

void server_loop(void);

#endif