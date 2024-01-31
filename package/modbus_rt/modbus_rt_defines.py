#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
 Modbus TestKit: Implementation of Modbus protocol in python

 (C)2009 - Luc Jean - luc.jean@gmail.com
 (C)2009 - Apidev - http://www.apidev.fr

 This is distributed under GNU LGPL license, see license.txt
"""

#modbus mode
SLAVE = 0
MASTER = 1

#modebus tcp type
SOCK_STREAM = 1
SOCK_DGRAM = 2

#modbus exception codes
ILLEGAL_FUNCTION = 1
ILLEGAL_DATA_ADDRESS = 2
ILLEGAL_DATA_VALUE = 3
SLAVE_DEVICE_FAILURE = 4
COMMAND_ACKNOWLEDGE = 5
SLAVE_DEVICE_BUSY = 6
MEMORY_PARITY_ERROR = 8

#supported modbus functions
READ_COILS = 1
READ_DISCRETE_INPUTS = 2
READ_HOLDING_REGISTERS = 3
READ_INPUT_REGISTERS = 4
WRITE_SINGLE_COIL = 5
WRITE_SINGLE_REGISTER = 6
READ_EXCEPTION_STATUS = 7
DIAGNOSTIC = 8
WRITE_MULTIPLE_COILS = 15
WRITE_MULTIPLE_REGISTERS = 16
REPORT_SLAVE_ID = 17
MASK_WRITE_REGISTER      = 22
READ_WRITE_MULTIPLE_REGISTERS = 23
DEVICE_INFO = 43

#supported block types
CIOLS = 0
INPUTS = 1
INPUT_REGISTERS = 3
REGISTERS = 4

#data trans endian mode
LITTLE_ENDIAL_SWAP = 0
BIG_ENDIAL_SWAP = 1
LITTLE_ENDIAL = 2
BIG_ENDIAL = 3
