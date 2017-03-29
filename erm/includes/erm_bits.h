/*
 * erm_bits.h © 2017 Michael David Täht
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Basic bit defines and conversions. C is really bad at dealing with bits */

#ifndef ERM_BITS_H
#define ERM_BITS_H

typedef struct {
  u8 v : 1;
} bit1;

typedef struct {
  u8 v : 2;
} bit2;

typedef struct {
  u8 v : 3;
} bit3;

typedef struct {
  u8 v : 4;
} bit4;

typedef struct {
  u8 v : 5;
} bit5;

typedef struct {
  u8 v : 6;
} bit6;

typedef struct {
  u8 v : 7;
} bit7;

typedef struct {
  u8 v : 8;
} bit8;

typedef struct {
  u16 v : 9;
} bit9;

typedef struct {
  u16 v : 10;
} bita;

typedef struct {
  u16 v : 11;
} bitb;

typedef struct {
  u16 v : 12;
} bitc;

typedef struct {
  u16 v : 13;
} bitd;

typedef struct {
  u16 v : 14
} bite;

typedef struct {
  u16 v : 15
} bitf;

typedef struct {
  u16 v : 16;
} bit16;

#endif
