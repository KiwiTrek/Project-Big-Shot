#include "ConfigJSON.h"
#include "parson.h"
#include "string.h"
#include "stdio.h"

// C++ wrapper for JSON parser library "Parson"

ConfigJSON::ConfigJSON()
{
	vroot = json_value_init_object();
	root = json_value_get_object(vroot);
	needs_removal = true;
}

ConfigJSON::ConfigJSON(const char* string)
{
	if (string != nullptr)
	{
		vroot = json_parse_string(string);
		if (vroot != nullptr) {
			root = json_value_get_object(vroot);
			needs_removal = true;
		}
	}
}

ConfigJSON::ConfigJSON(JSON_Object* section) : root(section)
{}

ConfigJSON::~ConfigJSON()
{
	if (needs_removal == true)
		json_value_free(vroot);
}

bool ConfigJSON::IsValid() const
{
	return root != nullptr;
}

size_t ConfigJSON::Save(char** buf, const char* title_comment) const
{
	size_t written = json_serialization_size(vroot);
	*buf = new char[written];
	json_serialize_to_buffer(vroot, *buf, written);
	return written;
}

int ConfigJSON::Size() const
{
	return (root) ? json_object_get_count(root) : 0;
}

ConfigJSON ConfigJSON::GetSection(const char* section_name) const
{
	return ConfigJSON(json_object_get_object(root, section_name));
}

ConfigJSON ConfigJSON::AddSection(const char* section_name)
{
	json_object_set_value(root, section_name, json_value_init_object());
	return GetSection(section_name);
}


JSON_Value* ConfigJSON::FindValue(const char* field, int index) const
{
	if (index < 0)
		return json_object_get_value(root, field);

	JSON_Array* array = json_object_get_array(root, field);
	if (array != nullptr)
		return json_array_get_value(array, index);

	return nullptr;
}

bool ConfigJSON::GetBool(const char* field, bool default, int index) const
{
	JSON_Value* value = FindValue(field, index);

	if (value && json_value_get_type(value) == JSONBoolean)
		return json_value_get_boolean(value) != 0;

	return default;
}

int ConfigJSON::GetInt(const char* field, int default, int index) const
{
	JSON_Value* value = FindValue(field, index);

	if (value && json_value_get_type(value) == JSONNumber)
		return (int)json_value_get_number(value);

	return default;
}

uint ConfigJSON::GetUInt(const char* field, uint default, int index) const
{
	JSON_Value* value = FindValue(field, index);

	if (value && json_value_get_type(value) == JSONNumber)
		return (uint)json_value_get_number(value);

	return default;
}

UID ConfigJSON::GetUID(const char* field, UID default, int index) const
{
	JSON_Value* value = FindValue(field, index);

	if (value && json_value_get_type(value) == JSONNumber)
		return (UID)json_value_get_number(value);

	return default;
}

double ConfigJSON::GetDouble(const char* field, double default, int index) const
{
	JSON_Value* value = FindValue(field, index);

	if (value && json_value_get_type(value) == JSONNumber)
		return json_value_get_number(value);

	return default;
}

float ConfigJSON::GetFloat(const char* field, float default, int index) const
{
	JSON_Value* value = FindValue(field, index);

	if (value && json_value_get_type(value) == JSONNumber)
		return (float)json_value_get_number(value);

	return default;
}

const char* ConfigJSON::GetString(const char* field, const char* default, int index) const
{
	JSON_Value* value = FindValue(field, index);

	if (value && json_value_get_type(value) == JSONString)
		return json_value_get_string(value);

	return default;
}

ConfigJSON ConfigJSON::GetArray(const char* field, int index) const
{
	JSON_Array* array = json_object_get_array(root, field);
	if (array != nullptr)
		return ConfigJSON(json_array_get_object(array, index));
	return ConfigJSON((JSON_Object*) nullptr);
}

int ConfigJSON::GetArrayCount(const char* field) const
{
	int ret = 0;
	JSON_Array* array = json_object_get_array(root, field);
	if (array != nullptr)
		ret = json_array_get_count(array);
	return ret;
}

bool ConfigJSON::AddBool(const char* field, bool value)
{
	return json_object_set_boolean(root, field, (value) ? 1 : 0) == JSONSuccess;
}

bool ConfigJSON::AddInt(const char* field, int value)
{
	return json_object_set_number(root, field, (double)value) == JSONSuccess;
}

bool ConfigJSON::AddUInt(const char* field, uint value)
{
	return json_object_set_number(root, field, (double)value) == JSONSuccess;
}

bool ConfigJSON::AddUID(const char* field, UID value)
{
	return json_object_set_number(root, field, (double)value) == JSONSuccess;
}

bool ConfigJSON::AddDouble(const char* field, double value)
{
	return json_object_set_number(root, field, value) == JSONSuccess;
}

bool ConfigJSON::AddFloat(const char* field, float value)
{
	return json_object_set_number(root, field, (float)value) == JSONSuccess;
}

bool ConfigJSON::AddString(const char* field, const char* string)
{
	return json_object_set_string(root, field, string) == JSONSuccess;
}

bool ConfigJSON::AddArray(const char* array_name)
{
	JSON_Value* va = json_value_init_array();
	array = json_value_get_array(va);

	return json_object_set_value(root, array_name, va) == JSONSuccess;
}

bool ConfigJSON::AddArrayEntry(const ConfigJSON& config)
{
	if (array != nullptr)
		return json_array_append_value(array, json_value_deep_copy(config.vroot)) == JSONSuccess;

	return false;
}

bool ConfigJSON::AddArrayBool(const char* field, const bool* values, int size)
{
	if (values != nullptr && size > 0)
	{
		JSON_Value* va = json_value_init_array();
		array = json_value_get_array(va);
		json_object_set_value(root, field, va);

		for (int i = 0; i < size; ++i)
			json_array_append_boolean(array, values[i]);
		return true;
	}
	return false;
}

bool ConfigJSON::AddArrayInt(const char* field, const int* values, int size)
{
	if (values != nullptr && size > 0)
	{
		JSON_Value* va = json_value_init_array();
		array = json_value_get_array(va);
		json_object_set_value(root, field, va);

		for (int i = 0; i < size; ++i)
			json_array_append_number(array, values[i]);
		return true;
	}
	return false;
}

bool ConfigJSON::AddArrayUInt(const char* field, const uint * values, int size)
{
	if (values != nullptr && size > 0)
	{
		JSON_Value* va = json_value_init_array();
		array = json_value_get_array(va);
		json_object_set_value(root, field, va);

		for (int i = 0; i < size; ++i)
			json_array_append_number(array, values[i]);
		return true;
	}
	return false;
}

bool ConfigJSON::AddArrayUID(const char* field, const UID * values, int size)
{
	if (values != nullptr && size > 0)
	{
		JSON_Value* va = json_value_init_array();
		array = json_value_get_array(va);
		json_object_set_value(root, field, va);

		for (int i = 0; i < size; ++i)
			json_array_append_number(array, (double)values[i]);
		return true;
	}
	return false;
}

bool ConfigJSON::AddArrayFloat(const char* field, const float* values, int size)
{
	if (values != nullptr && size > 0)
	{
		JSON_Value* va = json_value_init_array();
		array = json_value_get_array(va);
		json_object_set_value(root, field, va);

		for (int i = 0; i < size; ++i)
			json_array_append_number(array, values[i]);
		return true;
	}
	return false;
}

bool ConfigJSON::AddArrayFloat3(const char* field, const float3 * values, int size)
{
	if (values != nullptr && size > 0)
	{
		JSON_Value* va = json_value_init_array();
		array = json_value_get_array(va);
		json_object_set_value(root, field, va);

		for (int i = 0; i < size; ++i)
		{
			json_array_append_number(array, values[i].x);
			json_array_append_number(array, values[i].y);
			json_array_append_number(array, values[i].z);
		}
		return true;
	}
	return false;
}

bool ConfigJSON::AddArrayString(const char* field, const char** values, int size)
{
	if (values != nullptr && size > 0)
	{
		JSON_Value* va = json_value_init_array();
		array = json_value_get_array(va);
		json_object_set_value(root, field, va);

		for (int i = 0; i < size; ++i)
			json_array_append_string(array, values[i]);
		return true;
	}
	return false;
}

bool ConfigJSON::AddFloat2(const char* field, const float2 & value)
{
	return AddArrayFloat(field, &value.x, 2);
}

float2 ConfigJSON::GetFloat2(const char* field, const float2 & default /*= float2::zero*/)
{
	return float2(GetFloat(field, default.x, 0), GetFloat(field, default.y, 1));
}

bool ConfigJSON::AddFloat3(const char* field, const float3 & value)
{
	return AddArrayFloat(field, &value.x, 3);
}

float3 ConfigJSON::GetFloat3(const char* field, const float3 & default)
{
	return float3(
		GetFloat(field, default.x, 0),
		GetFloat(field, default.y, 1),
		GetFloat(field, default.z, 2));
}

bool ConfigJSON::AddFloat4(const char* field, const float4 & value)
{
	return AddArrayFloat(field, &value.x, 4);
}

float4 ConfigJSON::GetFloat4(const char* field, const float4 & default)
{
	return float4(
		GetFloat(field, default.x, 0),
		GetFloat(field, default.y, 1),
		GetFloat(field, default.z, 2),
		GetFloat(field, default.w, 3));
}