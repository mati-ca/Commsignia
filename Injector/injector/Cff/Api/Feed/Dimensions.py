# automatically generated by the FlatBuffers compiler, do not modify

# namespace: Feed

import flatbuffers

# /// Dimensions of Objects
class Dimensions(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsDimensions(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = Dimensions()
        x.Init(buf, n + offset)
        return x

    # Dimensions
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

# /// Length [m]
    # Dimensions
    def Length(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Float32Flags, o + self._tab.Pos)
        return 0.0

# /// Width [m]
    # Dimensions
    def Width(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(6))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Float32Flags, o + self._tab.Pos)
        return 0.0

def DimensionsStart(builder): builder.StartObject(2)
def DimensionsAddLength(builder, length): builder.PrependFloat32Slot(0, length, 0.0)
def DimensionsAddWidth(builder, width): builder.PrependFloat32Slot(1, width, 0.0)
def DimensionsEnd(builder): return builder.EndObject()
