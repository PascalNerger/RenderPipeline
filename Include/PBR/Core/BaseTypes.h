#pragma once


typedef unsigned char byte;
static_assert(sizeof(byte) == 1, "Invalid type size: byte");


typedef char int8;
typedef unsigned char uint8;
static_assert(sizeof(int8) == 1, "Invalid type size: int8");
static_assert(sizeof(uint8) == 1, "Invalid type size: uint8");

typedef short int16;
typedef unsigned short uint16;
static_assert(sizeof(int16) == 2, "Invalid type size: int16");
static_assert(sizeof(uint16) == 2, "Invalid type size: uint16");

typedef int int32;
typedef unsigned int uint32;
static_assert(sizeof(int32) == 4, "Invalid type size: int32");
static_assert(sizeof(uint32) == 4, "Invalid type size: uint32");

typedef long long int64;
typedef unsigned long long uint64;
static_assert(sizeof(int64) == 8, "Invalid type size: int64");
static_assert(sizeof(uint64) == 8, "Invalid type size: uint64");


static_assert(sizeof(float) == 4, "Invalid type size: float");
static_assert(sizeof(double) == 8, "Invalid type size: double");