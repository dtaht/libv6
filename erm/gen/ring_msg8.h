/*
 * erm_ringbuf.eh © 2017 Michael David Täht
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

// Natural u8 index type for ringbuffers of type msg8
// For single consumer/multiple producer.
// Elsewhere you would do something like this
// sed -i s/msg8/yourtype/g erm_ringbuf.eh |
// sed -i s/bit3/yourwidthtype/g erm_ringbuf.eh > yourtype.h

// msg8_ring_t msg8_i SECTION("ringbufs");
// msg8 msg8_r[256] SECTION("ringdata");

// void msg8_init() { msg8_r[] = { 0 }; }

inline bool msg8_is_full() { return (msg8_i.in.v - msg8_i.out.v ) == -1; }

inline bool msg8_is_empty() { return (msg8_i.in.v - msg8_i.out.v) == 0; }

inline bool msg8_is_hiwater()
{
  return (msg8_i.in.v - msg8_i.out.v) > (3 * 256 / 4);
}

inline bool msg8_is_lowater()
{
  return (msg8_i.in.v - msg8_i.out.v) > (256 / 4);
}

inline u8 msg8_size() { return (msg8_i.in.v - msg8_i.out.v); }

// Simplest possible locking enqueue and dequeue

inline void msg8_enqueue(msg8 value)
{
  msg8_r[(atomic_inc(msg8_i.in.v))] = value;
}

msg8 msg8_dequeue()
{
  return msg8_r[(atomic_inc(msg8_i.out.v))];
}

// Less locking push pop

inline bit3 msg8_copyto(bit3 qty, msg8* values)
{
  bit3 size,c,i;
  size.v = msg8_i.in.v - msg8_i.out.v;
  c.v = 0;
  i.v = msg8_i.out.v;
  if(size.v > qty.v) size.v = qty.v;
  for(; c.v < size.v; c.v++) values[c.v] = msg8_r[i.v + c.v];
  atomic_update(msg8_i.out.v, msg8_i.out.v + c.v);
  return c;
}

inline bit3 msg8_copyfrom(bit3 qty, msg8* values)
{
  bit3 size,c,i;
  size.v = (msg8_i.in.v - msg8_i.out.v) - 1;
  c.v = 0;
  i.v = msg8_i.in.v;
  if(size.v > qty.v) size.v = qty.v;
  for(; c.v < size.v; c.v++) msg8_r[i.v + c.v] = values[c.v];
  atomic_update(msg8_i.in.v, msg8_i.in.v + c.v);
  return c;
}

// Unsafe versions

inline void msg8_enqueue_unsafe(msg8 value)
{
  msg8_r[msg8_i.in.v++] = value;
}

msg8 msg8_dequeue_unsafe()
{
  return msg8_r[msg8_i.out.v++];
}
