#pragma once

#include <glad/glad.h>
#include "PBR/Core/BaseTypes.h"

static_assert(sizeof(GLchar) == sizeof(int8), "OpenGL type size mismatch: GLchar <-> int8");
static_assert(sizeof(GLbyte) == sizeof(int8), "OpenGL type size mismatch: GLbyte <-> int8");
static_assert(sizeof(GLubyte) == sizeof(uint8), "OpenGL type size mismatch: GLubyte <-> uint8");

static_assert(sizeof(GLshort) == sizeof(int16), "OpenGL type size mismatch: GLshort <-> int16");
static_assert(sizeof(GLushort) == sizeof(uint16), "OpenGL type size mismatch: GLushort <-> uint16");

static_assert(sizeof(GLenum) == sizeof(uint32), "OpenGL type size mismatch: GLenum <-> uint32");
static_assert(sizeof(GLint) == sizeof(int32), "OpenGL type size mismatch: GLint <-> int32");
static_assert(sizeof(GLuint) == sizeof(uint32), "OpenGL type size mismatch: GLuint <-> uint32");

static_assert(sizeof(GLint64) == sizeof(int64), "OpenGL type size mismatch: GLuint64 <-> int64");
static_assert(sizeof(GLuint64) == sizeof(uint64), "OpenGL type size mismatch: GLuint64 <-> uint64");

static_assert(sizeof(GLfloat) == sizeof(float), "OpenGL type size mismatch: GLfloat <-> float");
static_assert(sizeof(GLdouble) == sizeof(double), "OpenGL type size mismatch: GLdouble <-> double");
