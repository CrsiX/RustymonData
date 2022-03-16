import time
import uuid as _uuid
from typing import List, Tuple, Union

from . import __version__


class _Base(dict):
    def __init__(self, seq=None, **kwargs):
        super().__init__(seq, **kwargs)

    def __repr__(self) -> str:
        return f"{type(self).__name__}({super().__repr__()})"

    __str__ = __repr__

    def __getattr__(self, name):
        """
        Permit dictionary items to be retrieved like object attributes
        """

        try:
            return self[name]
        except KeyError:
            raise AttributeError(name)

    def __setattr__(self, name, value):
        """
        Permit dictionary items to be set like object attributes
        """

        self[name] = value

    def __delattr__(self, name):
        """
        Permit dictionary items to be deleted like object attributes
        """

        del self[name]
