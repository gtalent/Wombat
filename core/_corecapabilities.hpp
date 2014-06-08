/*
 * Copyright 2013-2014 gtalent2@gmail.com
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef WOMBAT_CORE__CORECAPABILITIES_HPP
#define WOMBAT_CORE__CORECAPABILITIES_HPP

// These defines are only for use inside core, other packages
//  should use the variables defined in corecapabilities.hpp.
#ifdef USE_SDL
	#define SUPPORTS_THREADS
#elif USE_GBA
#endif

#endif
