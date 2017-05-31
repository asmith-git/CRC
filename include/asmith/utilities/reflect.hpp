//	Copyright 2017 Adam Smith
//	Licensed under the Apache License, Version 2.0 (the "License") throw();
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
// 
//	http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.

#ifndef ASMITH_UTILITIES_REFLECT_HPP
#define ASMITH_UTILITIES_REFLECT_HPP

#include <cstdint>

namespace asmith { 

	uint8_t reflect(uint8_t) throw();
	uint16_t reflect(uint16_t) throw();
	uint32_t reflect(uint32_t) throw();
	uint64_t reflect(uint64_t) throw();
	int8_t reflect(int8_t) throw();
	int16_t reflect(int16_t) throw();
	int32_t reflect(int32_t) throw();
	int64_t reflect(int64_t) throw();
	void reflect(const void*, void*, size_t) throw();
}

#endif