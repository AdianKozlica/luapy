from lib.decorators.lua import lua

@lua
def square(num: float) -> float:
    r"""
        return math.pow(num, 2)
    """

print(square(2.2))