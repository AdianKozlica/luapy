#include <pybind11/pybind11.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <iostream>

namespace py = pybind11;

void close_lua(lua_State* L) {
    lua_pop(L, lua_gettop(L));
    lua_close(L);
}

py::object to_python_object(lua_State* L, int index) {
    py::object result_object = py::none();

    if (lua_isinteger(L, index)) {
        lua_Integer result = lua_tointeger(L, index);
        result_object = py::int_(result);
    }
    
    else if (lua_isnumber(L, index)) {
        lua_Number result = lua_tonumber(L, index);
        result_object = py::float_(result);
    }

    else if(lua_isboolean(L, index)) {
        int result = lua_toboolean(L, index);
        result_object = py::bool_(result);
    }

    else if (lua_isstring(L, index)) {
        const char* result = lua_tostring(L, index);
        result_object = py::str(result);
    }

    return result_object;
}

py::object run_lua(const char* lua_code, py::tuple args, const char* function_name) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_loadstring(L, lua_code) != LUA_OK) {
        lua_close(L);
        throw std::runtime_error("Code invalid");
    }

    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        lua_close(L);
        throw std::runtime_error("Code invalid");
    }

    lua_pop(L, lua_gettop(L));
    lua_getglobal(L, function_name);

    py::size_t args_size = args.size();   

    for (py::size_t i = 0; i < args_size; i++) {
        py::object obj = args[i];
   
        if (py::isinstance<py::int_>(obj)) {
            lua_pushinteger(L, obj.cast<long long>());
        }

        else if (py::isinstance<py::float_>(obj)) {
            lua_pushnumber(L, obj.cast<double>());
        }

        else if (py::isinstance<py::bool_>(obj)) {
            lua_pushboolean(L, obj.cast<bool>());
        }

        else if (py::isinstance<py::str>(obj)) {
            lua_pushstring(L, obj.cast<std::string>().c_str());
        }
    }

    if (lua_pcall(L, args_size, 1, 0) == LUA_OK) {
        if (lua_istable(L, -1)) {
            py::dict tmp_dict;
            lua_pushnil(L);
            while (lua_next(L, -2) != 0) {
                // the key is now at index -2, and the value at index -1
                py::object key = to_python_object(L, -2);
                py::object value = to_python_object(L, -1);
                tmp_dict[key] = value;
                lua_pop(L, 1);
            }

            close_lua(L);
            return tmp_dict;
        }

        py::object result = to_python_object(L, -1);
        lua_pop(L, 1);
        close_lua(L);
        return result;
    }
    
    close_lua(L);
    return py::none();
}

PYBIND11_MODULE(luapy, m) {
    m.def("run_lua", &run_lua, "A function that runs lua");
}