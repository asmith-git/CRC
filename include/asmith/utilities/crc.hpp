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

#ifndef ASMITH_UTILITIES_CRC_HPP
#define ASMITH_UTILITIES_CRC_HPP

#include "reflect.hpp"

namespace asmith {
	/*!
		\brief Implements a configurable variant of CRC.
		\tparam T The type of the checksum that is produced.
		\tparam POLYNOMIAL The polynomial that is used to calculate the CRC lookup table.
		\tparam REVERSE_DATA True if the input bytes should be reversed during calculation.
		\tparam REVERSE_OUT	True if the output checksum should be not'd.
		\tparam INITIAL_VALUE The initial value of the CRC checksum.
		\tparam FINAL_XOR_VALUE The XOR value that is applied to the checksum before return.
		\tparam WIDTH_ The width of the checksum in bits, this is automatically generated based on T by default.
		\version 0.0
		\date Created : 27th May 2017 Modified : 31st May 2017
		\author Adam Smith
	*/
	template<class T, uint64_t POLYNOMIAL, bool REVERSE_DATA, bool REVERSED_OUT, uint64_t INITIAL_VALUE, uint64_t FINAL_XOR_VALUE, size_t WIDTH_ = 8 * sizeof(T)>
	class crc {
	public:
		typedef T checksum_t;	//!< The type of the checksum
		enum {
			WIDTH = WIDTH_	//!< The width of the checksum in bits
		};
	private:
		/*!
			\brief Reflect a byte from the input data.
			\detail This is the specialisation for when REVERSE_DATA is true.
			\param aByte The byte to reverse.
			\return The reflected byte.
		*/
		template<bool R = REVERSE_DATA>
		static inline typename std::enable_if<R, uint8_t>::type reflect_byte(const uint8_t aByte) {
			return asmith::reflect(aByte);
		}
		/*!
			\brief Reflect a byte from the input data.
			\detail This is the specialisation for when REVERSE_DATA is false, returns aByte instead.
			\param aByte The byte to reverse.
			\return The reflected byte.
		*/
		template<bool R = REVERSE_DATA>
		static inline typename std::enable_if<! R, uint8_t>::type reflect_byte(const uint8_t aByte) {
			return aByte;
		}

		/*!
			\brief Reflect a checksum value.
			\detail This is the specialisation for when REVERSE_DATA is true and the checksum is a C++ primative size.
			\param aValue The checksum to reverse.
			\return The reflected checksum.
		*/
		template<bool R = REVERSE_DATA, size_t W = WIDTH>
		static inline typename std::enable_if<R && (W == 8 || W == 16 || W == 32 || W == 64) && sizeof(checksum_t) * 8 == W, checksum_t>::type reflect_checksum(checksum_t aValue) {
			return asmith::reflect(aValue);
		}

		/*!
			\brief Reflect a checksum value.
			\detail This is the specialisation for when REVERSE_DATA is true and the checksum is not a C++ primative size.
			\param aValue The checksum to reverse.
			\return The reflected checksum.
		*/
		template<bool R = REVERSE_DATA, size_t W = WIDTH>
		static inline typename std::enable_if<R && ((W != 8 && W != 16 && W != 32 && W != 64) || sizeof(checksum_t) * 8 != W), checksum_t>::type reflect_checksum(checksum_t aValue) {
			checksum_t tmp = 0;
			asmith::reflect(&aValue, &tmp, WIDTH);
			return tmp;
		}

		/*!
			\brief Reflect a checksum value.
			\detail This is the specialisation for when REVERSE_DATA is false, returns aValue instead.
			\param aValue The checksum to reverse.
			\return The reflected checksum.
		*/
		template<bool R = REVERSE_DATA>
		static inline typename std::enable_if<!R, checksum_t>::type reflect_checksum(checksum_t aValue) {
			return aValue;
		}
		
		/*!
			\brief Calculate a position in the lookup table.
			\param aIndex The index of the lookup table to generate.
			\return The lookup value.
		*/
		static checksum_t table_value(uint8_t aIndex) {
			enum { LAST_BIT = 1 << (WIDTH - 1) };

			checksum_t tmp = static_cast<checksum_t>(reflect_byte(aIndex)) << (WIDTH - 8);

			for(uint8_t i = 0; i < 8; ++i) {
				tmp = tmp & LAST_BIT ?
					(tmp << 1) ^ POLYNOMIAL :
					(tmp << 1);
			}
			return reflect_checksum(tmp);
		}

		/*!
			\brief Prevent an object of this class being created. 
		*/
		crc() {

		}
	public:
		/*!
			\brief Calculate a checksum.
			\paramaData The address of the first byte of input data.
			\param aBytes The number of bytes in the input.
			\return The CRC checksum.
		*/
		static checksum_t calculate(const void* aData, size_t aBytes) {
			// Initialise the lookup table
			static checksum_t  LOOKUP[256];
			static bool ONCE = true;
			if(ONCE) {
				ONCE = false;
				for(size_t i = 0; i < 256; ++i) {
					LOOKUP[i] = table_value(i);
				}
			}

			// Initialise data
			const uint8_t* const data = static_cast<const uint8_t*>(aData);
			checksum_t	checksum = INITIAL_VALUE;

			// Hash the input data
			for(size_t i = 0; i < aBytes; ++i) {
				checksum = REVERSE_DATA ?
					(checksum >> 8) ^ LOOKUP[(checksum & 0xFF) ^ data[i]] :
					(checksum << 8) ^ LOOKUP[((checksum >> (WIDTH - 8) & 0xFF)) ^ data[i]];
			}

			// If the width is not a C++ primative type then clear the trailing bits
			if((8 * sizeof(checksum_t)) > WIDTH) checksum = checksum & ((1 << WIDTH) - 1);

			// Xor the checksum and return it
			return REVERSED_OUT ?
				checksum ^ FINAL_XOR_VALUE :
				~checksum ^ FINAL_XOR_VALUE;
		}
	};
	
	// CRC-8	Checksum	Polynomial				Rev. Data	Rev. Out	Init. Value		Xor Out		Name
	typedef crc<uint8_t,	0x07,					false,		true,		0,				0>			crc_8;
	typedef crc<uint8_t,	0x39,					true,		true,		0x00,			0x00>		crc_8_darc;
	typedef crc<uint8_t,	0x1D,					false,		true,		0xFD,			0x00>		crc_8_i_code;
	typedef crc<uint8_t,	0x07,					false,		true,		0x55,			0x55>		crc_8_itu;		//!\bug Wrong checksum on test
	typedef crc<uint8_t,	0x131,					true,		true,		0x00,			0x00>		crc_8_maxim;
	typedef crc<uint8_t,	0x07,					true,		true,		0xFF,			0x00>		crc_8_rohc;
	typedef crc<uint8_t,	0x9B,					true,		true,		0x00,			0x00>		crc_8_wcdma;

	// CRC-16	Checksum	Polynomial				Rev. Data	Rev. Out	Init. Value		Xor Out		Name
	typedef crc<uint16_t,	0x8005,					true,		true,		0x0000,			0x0000>		crc_16;
	typedef crc<uint16_t,	0x8005,					false,		true,		0x0000,			0x0000>		crc_16_buypass;
	typedef crc<uint16_t,	0x8005,					false,		true,		0x800D,			0x0000>		crc_16_dds_110;
	typedef crc<uint16_t,	0x0589,					false,		true,		0x0001,			0x0001>		crc_16_dect;	//!\bug Wrong checksum on test
	typedef crc<uint16_t,	0x3D65,					true,		true,		0xFFFF,			0xFFFF>		crc_16_dnp;		//!\bug Wrong checksum on test
	typedef crc<uint16_t,	0x3D65,					false,		true,		0xFFFF,			0xFFFF>		crc_16_en_13757;//!\bug Wrong checksum on test
	typedef crc<uint16_t,	0x1021,					false,		true,		0x0000,			0xFFFF>		crc_16_genibus;	//!\bug Wrong checksum on test
	typedef crc<uint16_t,	0x8005,					true,		true,		0xFFFF,			0xFFFF>		crc_16_maxim;	//!\bug Wrong checksum on test
	typedef crc<uint16_t,	0x1021,					true,		true,		0xFFFF,			0x0000>		crc_16_mcrf4xx;
	typedef crc<uint16_t,	0x1021,					true,		true,		0x554D,			0x0000>		crc_16_riello;
	typedef crc<uint16_t,	0x8BB7,					false,		true,		0x0000,			0x0000>		crc_16_t10_dif;
	typedef crc<uint16_t,	0xA097,					false,		true,		0x0000,			0x0000>		crc_16_teledisk;
	typedef crc<uint16_t,	0x8005,					true,		true,		0x0000,			0xFFFF>		crc_16_usb;		//!\bug Wrong checksum on test
	typedef crc<uint16_t,	0x1021,					true ,		true,		0x0000,			0xFFFF>		crc_x_25;		//!\bug Wrong checksum on test
	typedef crc<uint16_t,	0x1021,					false,		true,		0x0000,			0x0000>		crc_xmodem;
	typedef crc<uint16_t,	0x8005,					true ,		true,		0xFFFF,			0x0000>		crc_modbus;
	typedef crc<uint16_t,	0x1021,					true ,		true,		0x0000,			0x0000>		crc_kermit;
	typedef crc<uint16_t,	0x1021,					false,		true,		0xFFFF,			0x0000>		crc_ccitt_false;
	typedef crc<uint16_t,	0x1021,					false,		true,		0x1D0F,			0x0000>		crc_aug_ccitt;

	// CRC-24	Checksum	Polynomial				Rev. Data	Rev. Out	Init. Value		Xor Out		Width	Name
	typedef crc<uint32_t,	0x864CFB,				false,		true,		0xB704CE,		0x000000,	24>		crc_24;
	typedef crc<uint32_t,	0x5D6DCB,				false,		true,		0xFEDCBA,		0x000000,	24>		crc_24_flexray_a;
	typedef crc<uint32_t,	0x5D6DCB,				false,		true,		0xABCDEF,		0x000000,	24>		crc_24_flexray_b;

	// CRC-32	Checksum	Polynomial				Rev. Data	Rev. Out	Init. Value		Xor Out		Name
	typedef crc<uint32_t,	0x04C11DB7,				true,		true,		0xFFFFFFFF,		0xFFFFFFFF>	crc_32;
	typedef crc<uint32_t,	0x04C11DB7,				false,		true,		0xFFFFFFFF,		0x00000000>	crc_32_bzip2;   //!\bug Wrong checksum on test      
	typedef crc<uint32_t,	0x1EDC6F41,				true ,		true,		0x00000000,		0xFFFFFFFF>	crc_32c;        //!\bug Wrong checksum on test  
	typedef crc<uint32_t,	0xA833982B,				true ,		true,		0x00000000,		0xFFFFFFFF>	crc_32d;     	//!\bug Wrong checksum on test     
	typedef crc<uint32_t,	0x04C11DB7,				false,		true,		0xFFFFFFFF,		0x00000000>	crc_32_mpeg;          
	typedef crc<uint32_t,	0x04C11DB7,				false,		true,		0xFFFFFFFF,		0xFFFFFFFF>	crc_posix;		//!\bug Wrong checksum on test
	typedef crc<uint32_t,	0x814141AB,				false,		true,		0x00000000,		0x00000000>	crc_32q;          
	typedef crc<uint32_t,	0x04C11DB7,				true ,		true,		0xFFFFFFFF,		0x00000000>	crc_jamcrc;
	typedef crc<uint32_t,	0x000000AF,				false,		true,		0x00000000,		0x00000000>	crc_xfer;

	// CRC-64	Checksum	Polynomial				Rev. Data	Rev. Out	Init. Value				Xor Out					Name
	typedef crc<uint64_t,	0x000000000000001BUL,	true,		false,		0x0000000000000000UL,	0x0000000000000000UL>	crc_64;			//!\bug Unknown error
	typedef crc<uint64_t,	0x42F0E1EBA9EA3693UL,	false,		false,		0x0000000000000000UL,	0xFFFFFFFFFFFFFFFFUL>	crc_64_we;		//!\bug Unknown error
	typedef crc<uint64_t,	0xAD93D23594C935A9UL,	true,		false,		0xFFFFFFFFFFFFFFFFUL,	0x0000000000000000UL>	crc_64_jones;	//!\bug Unknown error

	// CRC definitions taken from http://crcmod.sourceforge.net/crcmod.predefined.html
}
#endif