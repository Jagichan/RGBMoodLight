/*--------------------------------------------------------------------------------------
 EEPROM.H - Header file to support the EEPROM functions.
 Copyright (C) 2020 Jagannatha Rao (aka JagiChan) (jagannath_raous@yahoo.com)

 This program is free software: you can redistribute it and/or modify it under the terms
 of the version 3 GNU General Public License as published by the Free Software Foundation.
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.
 You should have received a copy of the GNU General Public License along with this program.
 If not, see <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------*/


#ifndef EEPROM_H
#define	EEPROM_H

unsigned char EEread(unsigned char addr);
void EEwrite(unsigned char addr, unsigned char data);

#endif	/* EEPROM_H */

