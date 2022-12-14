# automatically generated by the FlatBuffers compiler, do not modify

# namespace: Feed

import flatbuffers

# /// Electronics state of Objects
class Electronics(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsElectronics(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = Electronics()
        x.Init(buf, n + offset)
        return x

    # Electronics
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

# /// Left turn signal status
    # Electronics
    def LeftTurnSignal(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return bool(self._tab.Get(flatbuffers.number_types.BoolFlags, o + self._tab.Pos))
        return False

# /// Right turn signal status
    # Electronics
    def RightTurnSignal(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(6))
        if o != 0:
            return bool(self._tab.Get(flatbuffers.number_types.BoolFlags, o + self._tab.Pos))
        return False

def ElectronicsStart(builder): builder.StartObject(2)
def ElectronicsAddLeftTurnSignal(builder, leftTurnSignal): builder.PrependBoolSlot(0, leftTurnSignal, 0)
def ElectronicsAddRightTurnSignal(builder, rightTurnSignal): builder.PrependBoolSlot(1, rightTurnSignal, 0)
def ElectronicsEnd(builder): return builder.EndObject()
