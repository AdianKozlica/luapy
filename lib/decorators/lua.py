from luaparser import ast

from .. import luapy

def lua(fn):
    def inner(*args, **kwargs):
        body = ast.parse(fn.__doc__)
        fn_name = fn.__qualname__
        fn_args = list(fn.__code__.co_varnames)

        source_code = ast.to_lua_source(
            ast.Chunk(
                ast.Block(
                    [
                        ast.Function(fn_name, fn_args, body)
                    ]
                )
            )
        )

        return luapy.run_lua(source_code, args, fn_name)
    
    return inner
