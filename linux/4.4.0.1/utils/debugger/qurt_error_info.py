'''
    error_info.py

 GENERAL DESCRIPTION
        Script to print details related to Qurt Error codes.

 EXTERNAL FUNCTIONS
        None.

 INITIALIZATION AND SEQUENCING REQUIREMENTS
        None.

             Copyright (c)2020 by Qualcomm Technologies, Inc.  All Rights Reserved.

'''

#--------------------------------------------------------------------------------------------------------
#                          Description of QuRT_error_cause_codes.xlsx
#--------------------------------------------------------------------------------------------------------
'''

Sheet1: error_info

Columns: Cause,	Value, Cause details, Possible cause 2, Value, Cause2 Details, Badva, Event No

Sheet2: ssr_info

Columns: Event Number, Cause Code, Event Type, Event Description, Notes

'''

try:
    import xlrd
except:
    print("xlrd is not installed\n\t Please install xlrd version 1.2.0 to use this script")
    print("Installation command: pip3 install xlrd==1.2.0\nEnsure you use the pip tied to the python instance you are attempting to use.")

import sys


#--------------------------------------------------------------------------------------------------------
#                                       GLOBAL VARIABLES
#--------------------------------------------------------------------------------------------------------
'''

Defining following indices based on above columns info in input QuRT_error_cause_codes.xlsx

e prefix for columns in error_info sheet
s prefix for columns in ssr_info sheet

'''

DEBUG               = False
eCause              = 0
eValue              = 1
eCauseDetails       = 2
ePossibleCause2     = 3
eValue2             = 4
eCause2Details      = 5
eBadva              = 6
eEventNo            = 7

sEventNumber        = 0
sCauseCode          = 1
sEventType          = 2
sEventDescription   = 3
sNotes              = 4


#--------------------------------------------------------------------------------------------------------
#                                            FUNCTIONS
#--------------------------------------------------------------------------------------------------------

'''

Function: get_list

Convert sheet into list of lists

'''

def get_list(sheet):
    error_list = []
    for i in range(sheet.nrows):
        rows = []
        row = sheet.row(i)
        for cell in row:
            rows.append(cell.value)
        error_list.append(rows)
    return error_list

'''

Function: process_cause2

Capturing Possible cause2, Cause2 value, cause2 details and Event no (in case of SSR)
input: cause2 row(row) and dictionary to be filled(a)
return: processed dictionary(a)

'''

def process_cause2(row, cause2_dict):
    if row[eValue2] == '':
        if 'SSR' in row[ePossibleCause2]:
            index = 'SSR'
            cause2_dict[index] = [row[ePossibleCause2], row[eValue2], row[eCause2Details], row[eEventNo]]
        return cause2_dict
    index = hex(int(row[eValue2], 16))
    cause2_dict[index] = [row[ePossibleCause2], row[eCause2Details], row[eBadva]]
    return cause2_dict

'''

Function: process_error_rows

Capturing Cause 1, cause1 value, cause1 details
Input: Cause 1 current row (row), last processed row(last), Dictionary to be updated(error_list)
Return: Updated Dictionary (error_list)

'''

def process_error_rows(row, last = None, error_list= None):
    cause = row[eValue]
    if cause == '':
        #cause will be None in case there are multiple cause 2 rows corresponds to cause
        num = hex(int(last, 16))
        error_list[num][2] = (process_cause2(row, error_list[num][2]))
        return error_list
    cause = hex(int(cause, 16))
    error_list[cause] = [row[eCause], row[eCauseDetails], process_cause2(row, {})]
    return error_list

'''

Function: process_ssr_rows

Capturing SSR Event Number, Event Type, Event Description, Event number.
Input: SSR_sheet row (row), Dictionary to be filled(ssr_list)
Return: Processed Dictionary(ssr_list)

'''

def process_ssr_rows(row, ssr_list):
    ssr = row[sCauseCode]
    if (ssr is '') or ('#u8' in ssr):
        return ssr_list
    ssr_list[hex(int(ssr, 16))] = [row[sEventType], row[sEventDescription], row[sNotes], row[sEventNumber]]
    return ssr_list

'''

Function: create_error_dict

Main preprocessing function to generate error_info and ssr_info dictionary

Input: Excel sheet book(book)

Return: ssr_info and error_info processed dictionary

Description:
    error_info: type dictionary
        key: cause 1
        value: list [Cause1, Cause1 detail, cause2_dict]
            cause2_dict:
                    key: cause2
                    value: [cause2 value, cause2 details, badva]

    ssr_info: type dictionary
        key: SSR value
        value: list [eventType, EventDescription, notes, eventNumber]

    The error_info and ssr_info are the dictionaries corresponding to the sheets in the excel.

    For error_info, key will be the cause code 1 and values will be the row elements.
    For ssr_info, key will be the ssr code and value will be the rows respectively

    For Example:

    error_info:
        '0x10': ['QURT_EXCEPT_TLBMISS_RW_ISLAND\xa0 ',
            'RW Miss in island mode',
                {'0x70': ['QURT_TLB_MISS_RW_MEM_READ', '', ''],
                '0x71': ['QURT_TLB_MISS_RW_MEM_WRITE', '', '']}]

    ssr_info:
        0x0 ['Reset ', 'Software thread reset. ', 'Non Maskable,highest priority', 0]
        0x43 ['Imprecise', 'NMI', 'Non Maskable', 1]

'''

def create_error_dict(book):
    error_info = {}
    ssr_info = {}
    i = 0
    sheet1 = book.sheet_by_name('error_info')
    sheet2 = book.sheet_by_name('ssr_info')
    list1 = get_list(sheet1)
    list2 = get_list(sheet2)
    last = None
    for i in range(1, len(list1)):
        row = list1[i]
        error_info = process_error_rows(row, last, error_info)
        if row[eValue] is '':
            continue
        last = row[eValue]
    for i in range(1, len(list2)):
        row = list2[i]
        ssr_info = process_ssr_rows(row, ssr_info)
    return error_info, ssr_info

'''

Function: preprocess_data

Preprocessing function to read excel and return error_info and ssr_info

'''

def preprocess_data(filename):
    try:
        book = xlrd.open_workbook(filename)
    except:
        print("Your version of xlrd is not supported. xlrd 1.2.0 required")
        print("Installation command: pip3 install xlrd==1.2.0\nEnsure you use the pip tied to the python instance you are attempting to use.")
        exit(1)
    return create_error_dict(book)

'''

Function: check_for_error_codes

Function to check the input error code
Input: cause 1 and cause2
Return: True (valid input)/ False (invalid input)

'''

def check_for_error_codes(cause1, cause2, error_info, ssr_info):
    if cause1 in error_info:
        dict2 = error_info[cause1][2].keys()
        if cause2 in dict2:
            return True
        if ('SSR' in dict2):
            if ssr_info[cause2][3] == error_info[cause1][2]['SSR'][3]:
                return True
    print("Invalid cause code")
    return False

'''

Function: parse_error

Function to take cause as input, parse into cause1 and cause 2 and print out the details of exception occurred
Input: Cause code
Return: None

'''

def parse_error(cause, error_info, ssr_info):
    cause1 = hex(int(cause, 16) & int('0xFF', 16))
    cause2 = hex(int(cause, 16) & int('0xFF00', 16))[:-2]
    if check_for_error_codes(cause1, cause2, error_info, ssr_info) is False:
        return
    error_detail = error_info[cause1]
    print("Cause type: ", error_detail[0].strip(), "(", cause1, ")")
    print("Cause details: ", error_detail[1])
    print("Cause2 details: ")
    if 'SSR' in list(error_detail[2].keys()):
        ssr_error = ssr_info[cause2]
        print("Exception type: ", ssr_error[0].strip(), "(", cause2, ")")
        if ssr_error[1] is not None:
            print("Exception description: ", ssr_error[1])
    else:
        cause2_tmp = cause2
        cause2 = error_detail[2][cause2]
        print('Cause 2 error:', cause2[0].strip(), "(", cause2_tmp, ")")
        if cause2[1] is not None:
            print('Cause2 detail:', cause2[1])

'''

Function: print_error_dict

Debug Function to print the error_info and ssr_info

To print set DEBUG to True in global variables

'''

def print_error_dict(error_info, ssr_info):
    if DEBUG:
        from pprint import pprint
        pprint(error_info)
        pprint(ssr_info)

'''

Function: main

Staring function with following functionalities:
    1) Take user input
    2) Validate the input provided
    3) Prepare the data from excel
    4) Parse cause code provided

'''

def main():
    if sys.version_info[0] < 3:
        raise Exception("Please use python 3 for this script")
    if len(sys.argv) < 2:
        print("Wrong Usage")
        print("Correct usage: python3 error_info.py 0x600a")
        print("/* 0x600a is cause code */")
        print(sys.argv)
    else:
        filename = 'QuRT_error_cause_codes.xlsx'
        error_info, ssr_info = preprocess_data(filename)
        print_error_dict(error_info, ssr_info)
        parse_error(sys.argv[1], error_info, ssr_info)

main()
