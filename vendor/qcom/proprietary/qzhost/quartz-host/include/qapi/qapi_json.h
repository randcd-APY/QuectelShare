#ifndef __QAPI_JSON_BASE_H__
#define __QAPI_JSON_BASE_H__

/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "qapi_status.h"

/**
@file qapi_json.h
This section provides APIs, enumerations and data structures for applications to encode, decode, search, modify and delete JSON items.
*/

/**
@ingroup qapi_json  
Converts a given JSON formatted string into a JSON object and returns a handle for the object.

This handle is to be used for further operations, such as query, addition, deletion, and
replacement of items.

Applications are responsible for deleting the created objects. A failure to do so causes
memory leaks. Use the API qapi_JSON_Delete_Object() for this purpose.

Information regarding JSON format can be found at www.json.org. 
This API accepts a string in JSON format as input which can be JSON object or JSON array. 
Input can also be a single value, but this case doesn't have a practical use. 
All the subcomponents of the input which are of type string must be enclosed in escaped double quotes. 
Keys in key:value pairs must be strings, however, the values can be strings, integers, floating point numbers, 
null, true or false. 
Objects/arrays can also be nested by using one object/array as the value in key:value for another object/array. 
Values in key:value pairs which are of type string must be enclosed in escaped double quotes.
No formatting is needed for values of other types, including null, true and false.
\n
Following examples of input string demonstrate use of nested object, array and values of different types.\n
(1) {\\\"node\\\":\\\"washer\\\",\\\"soak\\\":true,\\\"time\\\":{\\\"hours\\\":1,\\\"minutes\\\":30.50}}\n
\n Note that every string in above input is enclosed in escaped double quotes.\n
(2) [\\\"value1\\\",2,3.5,true,false,null]

@param[in] input_String    JSON formatted string.
@param[in] handle          Pointer to the object handle returned to the application.

@sa
qapi_JSON_Delete_Object()\n
qapi_JSON_Get_Handle_List()

@return 
QAPI_OK -- JSON object creation succeeded.\n
Nonzero value -- JSON object creation failed.
*/

qapi_Status_t qapi_JSON_Decode(const char* input_String, uint32_t* handle);

/**
@ingroup qapi_json  
Converts a given JSON object into a JSON formatted string.

Applications must pass in the handle for the object to be converted and a pointer to retrieve 
the converted string. Applications are responsible for freeing the memory allocated for a new 
string. Standard library "free" functions can be used for this purpose.

@param[in] handle          Handle for the JSON object.
@param[out] output_String   Pointer to where to retrieve the JSON formatted string.

@return 
QAPI_OK -- JSON object was successfully converted into a string. \n
Nonzero value -- JSON object could not be converted to a string.
*/
qapi_Status_t qapi_JSON_Encode(uint32_t handle, char** output_String);

/**
@ingroup qapi_json  
Queries the specified key in a specified object and returns the value.
In the case of duplicate keys, this API returns the first occurrence.
The application must pass in a pointer to retrieve the value.\n
The query_Key doesn't need any formatting so escaped double quotes are not needed.

@param[in] handle          Handle for the JSON object.
@param[in] query_Key       Key to search for in the specified object.
@param[out] output_String   Item value for the given key, if found.

@return
QAPI_OK -- Specified key was found. \n
Nonzero value -- Key was not found.
*/
qapi_Status_t qapi_JSON_Query_By_Key(uint32_t handle, const char* query_Key, char** output_String);

/**
@ingroup qapi_json
Gets the value of an array item at a given index.\n
Applications must pass a pointer to retrieve the item's value.

@param[in] handle          Handle for the JSON object.
@param[in] index           Index at which the new item is to be inserted.
@param[out] output_String  Item value at the given index, if found.

@return
QAPI_OK -- Item was found at the given index. \n
Nonzero value -- Item was not found.
*/
qapi_Status_t qapi_JSON_Query_By_Index(uint32_t handle, uint32_t index, char** output_String);

/**
@ingroup qapi_json
This API is used to insert a new item at specified index in the specified JSON 
array. 
New item string must be JSON formatted, more information in the description for 
qapi_JSON_Decode().\n
As this API is specific for inserting entries with only value, 
not key:value, it is best used for arrays. 
Instead, the API qapi_JSON_Insert_KeyValue_By_Index() should be used for 
inserting key:value pairs. 
Note that API will not return error if used for objects however the result 
may not be as expected.
If the index is more than the length of existing object tree/array, then the 
item is appended at the end.\n
This API only works for the outermost level of nested structures. 
To insert a value in an array which is nested inside another arrays/objects, 
use qapi_JSON_Replace_Value_By_Index() instead of this API to replace the 
whole object/array which is at the outermost level.

@param[in] handle          Handle for the JSON object.
@param[in] index           Index at which the new item is to be inserted.
@param[in] value           Value for new item to be added.

@return
QAPI_OK -- Specified item was deleted.\n
Nonzero value -- Item with the specified key was not found.
*/
qapi_Status_t qapi_JSON_Insert_Value_By_Index(uint32_t handle, uint32_t index, const char* value);

/**
@ingroup qapi_json
Inserts a new item at a given index in the object.

Key input does NOT need any formatting so escaped double quotes are not needed. 
However, the value input must adhere to JSON format string for the value, more information 
in the description for qapi_JSON_Decode().
The API only works for the outermost level of nested structures. 
To insert an entry in an object which is nested inside another array/object, 
use qapi_JSON_Replace_Value_By_Index(), qapi_JSON_Replace_Value_By_Key() 
instead of this API to replace the whole object/array which is at the outermost level.


@param[in] handle          Handle for the JSON object.
@param[in] index           Index at which the new item is to be inserted.
@param[in] key             Key for the new item to be added.
@param[in] value           Value for the new item to be added.

@return
QAPI_OK -- Specified item was inserted.\n
Nonzero value -- Item with the specified key was not found.
*/
qapi_Status_t qapi_JSON_Insert_KeyValue_By_Index(uint32_t handle, uint32_t index, const char* key, const char* value);

/**
@ingroup qapi_json
This API is used to replace value of an existing item at given index in 
specified JSON object/JSON array with a new value. 
The API returns an error if the index is more than the length of 
existing object tree/array.
New value string must be JSON formatted, more information 
in the description for qapi_JSON_Decode(). 
If used for JSON object, original key field is retained.
The API only works for the outermost level of nested structures.
For replacing values/objects/arrays in inner levels of nested objects, 
replace the complete array/object at the outermost level which includes 
the value to be replaced.

Example: For following object, inserting a new value in nested array or changing one value 
from the array is not possible directly as the handle provided is for the outermost object. 
Instead the whole array can be replaced with the expected value.
\n{
\n"key1": "value1",
\n"key2": [0, 1, 2],
\n"key3": null
\n}\n

(1) Replace value 1 from array to 5.\n
Input value: [0,5,2].\n\n

(2) Insert new values in the array.\n
Input value: [0,5,2,null,\\\"NewValue\\\"]


@param[in] handle          Handle for the JSON object.
@param[in] index           Index of the item to be replaced.
@param[in] new_Value       New value for the specified key.

@return
QAPI_OK -- Specified item was replaced.\n
Nonzero value -- Item at the given index was not found.
*/
qapi_Status_t qapi_JSON_Replace_Value_By_Index(uint32_t handle, uint32_t index, const char* new_Value);

/**
@ingroup qapi_json
This API is used to replace value of an existing item with given key in 
specified JSON objects/arrays with a new value. 
API returns an error if an item with given key is not found. 
New value string must be JSON formatted, more information 
in the description for qapi_JSON_Decode().
In case of repeated keys, only first occurrence is modified.
This API works only on outermost level of nested objects. 
To insert/modify an entry in an object nested inside another object/array, 
replace the complete outermost array. Similar examples given in description of 
qapi_JSON_Replace_Value_By_Index(). 

@param[in] handle          Handle for the JSON object.
@param[in] query_Key       Key to search for in the specified object.
@param[in] new_Value       New value for the specified key.

@return
QAPI_OK -- Specified item was replaced.\n
Nonzero value -- Item with the specified key was not found.
*/
qapi_Status_t qapi_JSON_Replace_Value_By_Key(uint32_t handle, const char* query_Key, const char* new_Value);

/**
@ingroup qapi_json
Deletes an item at a given index in an array. To delete an entry in an 
object/array nested inside another, use qapi_JSON_Replace_Value_By_Index() or 
qapi_JSON_Replace_Value_By_Index().



@param[in] handle          Handle for the JSON object.
@param[in] index           Index of the item to be deleted.

@return
QAPI_OK -- Specified item was deleted.\n
Nonzero value -- Item at the given index was not found.
*/
qapi_Status_t qapi_JSON_Delete_Entry_By_Index(uint32_t handle, uint32_t index);

/**
@ingroup qapi_json
Deletes an item with a specified key from the JSON object tree. 
In the case of duplicate keys, this API deletes the first item. 
To delete an entry in an object/array nested inside another, use 
qapi_JSON_Replace_Value_By_Index() or qapi_JSON_Replace_Value_By_Index().


@param[in] handle          Handle for the JSON object.
@param[in] query_Key       Key to search for in the specified object.

@return
QAPI_OK -- Specified item was deleted.\n
Nonzero value -- Item with the specified key was not found.
*/
qapi_Status_t qapi_JSON_Delete_Entry_By_Key(uint32_t handle, const char* query_Key);

/**
@ingroup qapi_json
Deletes a complete json object tree.
 
Applications should use the API to delete every object tree created using qapi_JSON_Decode() to free the allocated memory.

@param[in] handle          Handle for the JSON object for deleting individual objects.
@param[in] all             If this parameter is set to 1, the API deletes all created object trees. Otherwise, if set to 0, only the tree with specified handle is deleted.

@return
QAPI_OK -- Object was deleted.\n
Nonzero value -- Object was not found.
*/
qapi_Status_t qapi_JSON_Delete_Object(uint32_t handle, uint32_t all);


/**
@ingroup qapi_json
Gets a list of all created objects.

@param[in] list    Pointer to where to retrieve the list of created objects..
@param[in] size    Pointer to the size of the list of created objects.

@return
QAPI_OK -- Success in getting the list of created objects.\n
Nonzero value -- Error in getting the list of created objects.
*/
qapi_Status_t qapi_JSON_Get_Handle_List(uint32_t **list, uint32_t *size);

#endif // __QAPI_JSON_H__
