/*
 * erm_logger.h © 2017 Michael David Täht
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
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

#ifndef LOGGER_H
#define LOGGER_H
#define tlogger(a, b, ...) tlog(a, b, __LINE__, __func__, ...)

extern int tprint(int dest, ...);
extern int tlog(int errlevel, int err, int line, char* file, char* func, ...);

// Logging is done both on severity and on type or file basis
#ifdef DEBUG
#define debug_log(a, b, ...)                                                 \
  a < DEBUG&& b & ~DEBUG ? DONOTHING : tlogger(errlevel, err, ...)
#define debug_log_address(a, b, ...)                                         \
  a < DEBUG&& b & ~DEBUG ? DONOTHING : tlogger(errlevel, err, ...)
#else
#define debug_log(a, b, ...) DONOTHING
#define debug_log_address(a, b, ...) DONOTHING
#endif

#endif
