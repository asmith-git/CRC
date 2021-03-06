//	Copyright 2017 Adam Smith
//	Licensed under the Apache License, Version 2.0 (the "License");
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

#include "asmith/utilities/reflect.hpp"
#include <algorithm>

namespace asmith {

	static constexpr uint8_t REFLECTION_LOOKUP[256] = {
		0b00000000, // 00000000 (0)
		0b10000000, // 00000001 (1)
		0b01000000, // 00000010 (2)
		0b11000000, // 00000011 (3)
		0b00100000, // 00000100 (4)
		0b10100000, // 00000101 (5)
		0b01100000, // 00000110 (6)
		0b11100000, // 00000111 (7)
		0b00010000, // 00001000 (8)
		0b10010000, // 00001001 (9)
		0b01010000, // 00001010 (10)
		0b11010000, // 00001011 (11)
		0b00110000, // 00001100 (12)
		0b10110000, // 00001101 (13)
		0b01110000, // 00001110 (14)
		0b11110000, // 00001111 (15)
		0b00001000, // 00010000 (16)
		0b10001000, // 00010001 (17)
		0b01001000, // 00010010 (18)
		0b11001000, // 00010011 (19)
		0b00101000, // 00010100 (20)
		0b10101000, // 00010101 (21)
		0b01101000, // 00010110 (22)
		0b11101000, // 00010111 (23)
		0b00011000, // 00011000 (24)
		0b10011000, // 00011001 (25)
		0b01011000, // 00011010 (26)
		0b11011000, // 00011011 (27)
		0b00111000, // 00011100 (28)
		0b10111000, // 00011101 (29)
		0b01111000, // 00011110 (30)
		0b11111000, // 00011111 (31)
		0b00000100, // 00100000 (32)
		0b10000100, // 00100001 (33)
		0b01000100, // 00100010 (34)
		0b11000100, // 00100011 (35)
		0b00100100, // 00100100 (36)
		0b10100100, // 00100101 (37)
		0b01100100, // 00100110 (38)
		0b11100100, // 00100111 (39)
		0b00010100, // 00101000 (40)
		0b10010100, // 00101001 (41)
		0b01010100, // 00101010 (42)
		0b11010100, // 00101011 (43)
		0b00110100, // 00101100 (44)
		0b10110100, // 00101101 (45)
		0b01110100, // 00101110 (46)
		0b11110100, // 00101111 (47)
		0b00001100, // 00110000 (48)
		0b10001100, // 00110001 (49)
		0b01001100, // 00110010 (50)
		0b11001100, // 00110011 (51)
		0b00101100, // 00110100 (52)
		0b10101100, // 00110101 (53)
		0b01101100, // 00110110 (54)
		0b11101100, // 00110111 (55)
		0b00011100, // 00111000 (56)
		0b10011100, // 00111001 (57)
		0b01011100, // 00111010 (58)
		0b11011100, // 00111011 (59)
		0b00111100, // 00111100 (60)
		0b10111100, // 00111101 (61)
		0b01111100, // 00111110 (62)
		0b11111100, // 00111111 (63)
		0b00000010, // 01000000 (64)
		0b10000010, // 01000001 (65)
		0b01000010, // 01000010 (66)
		0b11000010, // 01000011 (67)
		0b00100010, // 01000100 (68)
		0b10100010, // 01000101 (69)
		0b01100010, // 01000110 (70)
		0b11100010, // 01000111 (71)
		0b00010010, // 01001000 (72)
		0b10010010, // 01001001 (73)
		0b01010010, // 01001010 (74)
		0b11010010, // 01001011 (75)
		0b00110010, // 01001100 (76)
		0b10110010, // 01001101 (77)
		0b01110010, // 01001110 (78)
		0b11110010, // 01001111 (79)
		0b00001010, // 01010000 (80)
		0b10001010, // 01010001 (81)
		0b01001010, // 01010010 (82)
		0b11001010, // 01010011 (83)
		0b00101010, // 01010100 (84)
		0b10101010, // 01010101 (85)
		0b01101010, // 01010110 (86)
		0b11101010, // 01010111 (87)
		0b00011010, // 01011000 (88)
		0b10011010, // 01011001 (89)
		0b01011010, // 01011010 (90)
		0b11011010, // 01011011 (91)
		0b00111010, // 01011100 (92)
		0b10111010, // 01011101 (93)
		0b01111010, // 01011110 (94)
		0b11111010, // 01011111 (95)
		0b00000110, // 01100000 (96)
		0b10000110, // 01100001 (97)
		0b01000110, // 01100010 (98)
		0b11000110, // 01100011 (99)
		0b00100110, // 01100100 (100)
		0b10100110, // 01100101 (101)
		0b01100110, // 01100110 (102)
		0b11100110, // 01100111 (103)
		0b00010110, // 01101000 (104)
		0b10010110, // 01101001 (105)
		0b01010110, // 01101010 (106)
		0b11010110, // 01101011 (107)
		0b00110110, // 01101100 (108)
		0b10110110, // 01101101 (109)
		0b01110110, // 01101110 (110)
		0b11110110, // 01101111 (111)
		0b00001110, // 01110000 (112)
		0b10001110, // 01110001 (113)
		0b01001110, // 01110010 (114)
		0b11001110, // 01110011 (115)
		0b00101110, // 01110100 (116)
		0b10101110, // 01110101 (117)
		0b01101110, // 01110110 (118)
		0b11101110, // 01110111 (119)
		0b00011110, // 01111000 (120)
		0b10011110, // 01111001 (121)
		0b01011110, // 01111010 (122)
		0b11011110, // 01111011 (123)
		0b00111110, // 01111100 (124)
		0b10111110, // 01111101 (125)
		0b01111110, // 01111110 (126)
		0b11111110, // 01111111 (127)
		0b00000001, // 10000000 (128)
		0b10000001, // 10000001 (129)
		0b01000001, // 10000010 (130)
		0b11000001, // 10000011 (131)
		0b00100001, // 10000100 (132)
		0b10100001, // 10000101 (133)
		0b01100001, // 10000110 (134)
		0b11100001, // 10000111 (135)
		0b00010001, // 10001000 (136)
		0b10010001, // 10001001 (137)
		0b01010001, // 10001010 (138)
		0b11010001, // 10001011 (139)
		0b00110001, // 10001100 (140)
		0b10110001, // 10001101 (141)
		0b01110001, // 10001110 (142)
		0b11110001, // 10001111 (143)
		0b00001001, // 10010000 (144)
		0b10001001, // 10010001 (145)
		0b01001001, // 10010010 (146)
		0b11001001, // 10010011 (147)
		0b00101001, // 10010100 (148)
		0b10101001, // 10010101 (149)
		0b01101001, // 10010110 (150)
		0b11101001, // 10010111 (151)
		0b00011001, // 10011000 (152)
		0b10011001, // 10011001 (153)
		0b01011001, // 10011010 (154)
		0b11011001, // 10011011 (155)
		0b00111001, // 10011100 (156)
		0b10111001, // 10011101 (157)
		0b01111001, // 10011110 (158)
		0b11111001, // 10011111 (159)
		0b00000101, // 10100000 (160)
		0b10000101, // 10100001 (161)
		0b01000101, // 10100010 (162)
		0b11000101, // 10100011 (163)
		0b00100101, // 10100100 (164)
		0b10100101, // 10100101 (165)
		0b01100101, // 10100110 (166)
		0b11100101, // 10100111 (167)
		0b00010101, // 10101000 (168)
		0b10010101, // 10101001 (169)
		0b01010101, // 10101010 (170)
		0b11010101, // 10101011 (171)
		0b00110101, // 10101100 (172)
		0b10110101, // 10101101 (173)
		0b01110101, // 10101110 (174)
		0b11110101, // 10101111 (175)
		0b00001101, // 10110000 (176)
		0b10001101, // 10110001 (177)
		0b01001101, // 10110010 (178)
		0b11001101, // 10110011 (179)
		0b00101101, // 10110100 (180)
		0b10101101, // 10110101 (181)
		0b01101101, // 10110110 (182)
		0b11101101, // 10110111 (183)
		0b00011101, // 10111000 (184)
		0b10011101, // 10111001 (185)
		0b01011101, // 10111010 (186)
		0b11011101, // 10111011 (187)
		0b00111101, // 10111100 (188)
		0b10111101, // 10111101 (189)
		0b01111101, // 10111110 (190)
		0b11111101, // 10111111 (191)
		0b00000011, // 11000000 (192)
		0b10000011, // 11000001 (193)
		0b01000011, // 11000010 (194)
		0b11000011, // 11000011 (195)
		0b00100011, // 11000100 (196)
		0b10100011, // 11000101 (197)
		0b01100011, // 11000110 (198)
		0b11100011, // 11000111 (199)
		0b00010011, // 11001000 (200)
		0b10010011, // 11001001 (201)
		0b01010011, // 11001010 (202)
		0b11010011, // 11001011 (203)
		0b00110011, // 11001100 (204)
		0b10110011, // 11001101 (205)
		0b01110011, // 11001110 (206)
		0b11110011, // 11001111 (207)
		0b00001011, // 11010000 (208)
		0b10001011, // 11010001 (209)
		0b01001011, // 11010010 (210)
		0b11001011, // 11010011 (211)
		0b00101011, // 11010100 (212)
		0b10101011, // 11010101 (213)
		0b01101011, // 11010110 (214)
		0b11101011, // 11010111 (215)
		0b00011011, // 11011000 (216)
		0b10011011, // 11011001 (217)
		0b01011011, // 11011010 (218)
		0b11011011, // 11011011 (219)
		0b00111011, // 11011100 (220)
		0b10111011, // 11011101 (221)
		0b01111011, // 11011110 (222)
		0b11111011, // 11011111 (223)
		0b00000111, // 11100000 (224)
		0b10000111, // 11100001 (225)
		0b01000111, // 11100010 (226)
		0b11000111, // 11100011 (227)
		0b00100111, // 11100100 (228)
		0b10100111, // 11100101 (229)
		0b01100111, // 11100110 (230)
		0b11100111, // 11100111 (231)
		0b00010111, // 11101000 (232)
		0b10010111, // 11101001 (233)
		0b01010111, // 11101010 (234)
		0b11010111, // 11101011 (235)
		0b00110111, // 11101100 (236)
		0b10110111, // 11101101 (237)
		0b01110111, // 11101110 (238)
		0b11110111, // 11101111 (239)
		0b00001111, // 11110000 (240)
		0b10001111, // 11110001 (241)
		0b01001111, // 11110010 (242)
		0b11001111, // 11110011 (243)
		0b00101111, // 11110100 (244)
		0b10101111, // 11110101 (245)
		0b01101111, // 11110110 (246)
		0b11101111, // 11110111 (247)
		0b00011111, // 11111000 (248)
		0b10011111, // 11111001 (249)
		0b01011111, // 11111010 (250)
		0b11011111, // 11111011 (251)
		0b00111111, // 11111100 (252)
		0b10111111, // 11111101 (253)
		0b01111111, // 11111110 (254)
		0b11111111  // 11111111 (255)
	};

	/*!
		\brief Reverse the bit order of an unsigned 8 bit value.
		\param aValue The value to reflect.
		\return The reflected value.
	*/
	uint8_t reflect(uint8_t aValue) throw() {
		return REFLECTION_LOOKUP[aValue];
	}

	/*!
		\brief Reverse the bit order of an unsigned 16 bit value.
		\param aValue The value to reflect.
		\return The reflected value.
	*/
	uint16_t reflect(uint16_t aValue) throw() {
		uint16_t tmp;
		uint8_t* const dst = reinterpret_cast<uint8_t*>(&tmp);
		const uint8_t* const src = reinterpret_cast<const uint8_t*>(&aValue);
		dst[0] = REFLECTION_LOOKUP[src[1]];
		dst[1] = REFLECTION_LOOKUP[src[0]];
		return tmp;
	}

	/*!
		\brief Reverse the bit order of an unsigned 32 bit value.
		\param aValue The value to reflect.
		\return The reflected value.
	*/
	uint32_t reflect(uint32_t aValue) throw() {
		uint32_t tmp;
		uint8_t* const dst = reinterpret_cast<uint8_t*>(&tmp);
		const uint8_t* const src = reinterpret_cast<const uint8_t*>(&aValue);
		dst[0] = REFLECTION_LOOKUP[src[3]];
		dst[1] = REFLECTION_LOOKUP[src[2]];
		dst[2] = REFLECTION_LOOKUP[src[1]];
		dst[3] = REFLECTION_LOOKUP[src[0]];
		return tmp;
	}

	/*!
		\brief Reverse the bit order of an unsigned 64 bit value.
		\param aValue The value to reflect.
		\return The reflected value.
	*/
	uint64_t reflect(uint64_t aValue) throw() {
		uint64_t tmp;
		uint8_t* const dst = reinterpret_cast<uint8_t*>(&tmp);
		const uint8_t* const src = reinterpret_cast<const uint8_t*>(&aValue);
		dst[0] = REFLECTION_LOOKUP[src[7]];
		dst[1] = REFLECTION_LOOKUP[src[6]];
		dst[2] = REFLECTION_LOOKUP[src[5]];
		dst[3] = REFLECTION_LOOKUP[src[4]];
		dst[4] = REFLECTION_LOOKUP[src[3]];
		dst[5] = REFLECTION_LOOKUP[src[2]];
		dst[6] = REFLECTION_LOOKUP[src[1]];
		dst[7] = REFLECTION_LOOKUP[src[0]];
		return tmp;
	}

	/*!
		\brief Reverse the bit order of a signed 8 bit value.
		\param aValue The value to reflect.
		\return The reflected value.
	*/
	int8_t reflect(int8_t aValue) throw() {
		int8_t tmp;
		uint8_t* const dst = reinterpret_cast<uint8_t*>(&tmp);
		const uint8_t* const src = reinterpret_cast<const uint8_t*>(&aValue);
		dst[0] = REFLECTION_LOOKUP[src[0]];
		return tmp;
	}

	/*!
		\brief Reverse the bit order of a signed 16 bit value.
		\param aValue The value to reflect.
		\return The reflected value.
	*/
	int16_t reflect(int16_t aValue) throw() {
		int16_t tmp;
		uint8_t* const dst = reinterpret_cast<uint8_t*>(&tmp);
		const uint8_t* const src = reinterpret_cast<const uint8_t*>(&aValue);
		dst[0] = REFLECTION_LOOKUP[src[1]];
		dst[1] = REFLECTION_LOOKUP[src[0]];
		return tmp;
	}

	/*!
		\brief Reverse the bit order of a signed 32 bit value.
		\param aValue The value to reflect.
		\return The reflected value.
	*/
	int32_t reflect(int32_t aValue) throw() {
		int32_t tmp;
		uint8_t* const dst = reinterpret_cast<uint8_t*>(&tmp);
		const uint8_t* const src = reinterpret_cast<const uint8_t*>(&aValue);
		dst[0] = REFLECTION_LOOKUP[src[3]];
		dst[1] = REFLECTION_LOOKUP[src[2]];
		dst[2] = REFLECTION_LOOKUP[src[1]];
		dst[3] = REFLECTION_LOOKUP[src[0]];
		return tmp;
	}

	/*!
		\brief Reverse the bit order of a signed 64 bit value.
		\param aValue The value to reflect.
		\return The reflected value.
	*/
	int64_t reflect(int64_t aValue) throw() {
		int64_t tmp;
		uint8_t* const dst = reinterpret_cast<uint8_t*>(&tmp);
		const uint8_t* const src = reinterpret_cast<const uint8_t*>(&aValue);
		dst[0] = REFLECTION_LOOKUP[src[7]];
		dst[1] = REFLECTION_LOOKUP[src[6]];
		dst[2] = REFLECTION_LOOKUP[src[5]];
		dst[3] = REFLECTION_LOOKUP[src[4]];
		dst[4] = REFLECTION_LOOKUP[src[3]];
		dst[5] = REFLECTION_LOOKUP[src[2]];
		dst[6] = REFLECTION_LOOKUP[src[1]];
		dst[7] = REFLECTION_LOOKUP[src[0]];
		return tmp;
	}

	/*!
		\brief Reverse the bit order of an arbitrary length value.
		\param aInput The first byte of the value to reflect.
		\param aOutput The first byte of the reflected value.
		\param aBits The number of bits to reflect.
	*/
	void reflect(const void* aInput, void* aOutput, size_t aBits) throw() {
		// Reflect byte values
		const uint8_t* const src = reinterpret_cast<const uint8_t*>(aInput);
		uint8_t* const dst = reinterpret_cast<uint8_t*>(aOutput);
		size_t byte = 0;

		while(aBits >= 8) {
			aBits -= 8;
			dst[byte] = REFLECTION_LOOKUP[src[byte]];
			++byte;
		}

		if(aBits > 0) dst[byte] = 0;
		for(uint8_t i = 0; i < aBits; ++i) {
			if((src[byte] & 1) == 1) dst[byte] |= (1 << ((aBits - 1) - i));
			dst[byte] = dst[byte] >> 1;
		}

		// Reverse byte order
		std::reverse(dst, dst+byte);
	}
}