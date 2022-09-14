# #!/usr/bin/env /rwdata/python/bin/python3

import flatbuffers
import socket
import time
import sys
import copy

from Cff.Api.Feed.ObjectType import *
from Cff.Api.Feed.VehicleType import *
from Cff.Api.Feed.Dimensions import *
from Cff.Api.Feed.Motion import *
from Cff.Api.Feed.Position import *
from Cff.Api.Feed.Electronics import *
from Cff.Api.Feed.Object import *
from Cff.Api.Feed.DataElement import *
from Cff.Api.Feed.Input import *


def inject(config, obj_data):

    builder = flatbuffers.Builder(1024)

    DimensionsStart(builder)
    DimensionsAddLength(builder, 5.3)
    DimensionsAddWidth(builder, 2.1)
    dim = DimensionsEnd(builder)

    MotionStart(builder)
    MotionAddPosition(builder, CreatePosition(builder, obj_data.lat, obj_data.lon))
    MotionAddSpeed(builder, obj_data.speed)
    MotionAddAcceleration(builder, obj_data.acceleration)
    MotionAddHeading(builder, obj_data.heading)
    MotionAddYawRate(builder, obj_data.yawRate)
    motion = MotionEnd(builder)

    ObjectStart(builder)
    ObjectAddId(builder, obj_data.id)
    ObjectAddObjectType(builder, obj_data.type)
    ObjectAddDimensions(builder, dim)
    ObjectAddMotion(builder, motion)
    obj = ObjectEnd(builder)

    InputStart(builder)
    InputAddTimestamp(builder, int(round(time.time() * 1000)))
    InputAddDataType(builder, DataElement().Object)
    InputAddData(builder, obj)
    inputData = InputEnd(builder)

    builder.Finish(inputData)
    data = builder.Output()

    # send data over UDP
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP
    sock.sendto(data, (config.ip, config.port))

    print(f'Object injected with ID {obj_data.id} lat {obj_data.lat} lon {obj_data.lon} hdg {obj_data.heading}')

class Configuration():
    def __init__(self, ip="127.0.0.1", port=12321, timestep=0.1):
        self.ip = ip
        self.port = port
        self.timestep = timestep


class ObjectFeedBuilder:
    def __init__(self, src, dst, number):
        self._src_lat = src[0]
        self._src_lon = src[1]
        self._dst_lat = dst[0]
        self._dst_lon = dst[1]
        self._number = number

    def generate(self, id, data_skeleton, delta_lat=0.0, delta_lon=0.0):
        ret = []
        for i in range(self._number):
            lat = (i / self._number) * self._dst_lat + (self._number - i) / self._number * self._src_lat + delta_lat
            lon = (i / self._number) * self._dst_lon + (self._number - i) / self._number * self._src_lon + delta_lon
            ret += [data_skeleton.build(id, lat, lon)]
        return ret


class ObjectFeedData:
    def __init__(self, objectType, speed=0, acceleration=0, heading=0, yawRate=0):
        self.lat = None
        self.lon = None
        self.id = None
        self.speed = speed
        self.acceleration = acceleration
        self.heading = heading
        self.yawRate = yawRate
        self.type = objectType

    def build(self, id, lat, lon):
        self.lat = lat
        self.lon = lon
        self.id = id
        return copy.deepcopy(self)


if __name__ == "__main__":

    config = Configuration(ip="192.168.1.55")

    builder = ObjectFeedBuilder((60.163337, 24.915767), (60.163277, 24.914960), 300)

    skeleton = ObjectFeedData(ObjectType().Cyclist, speed=2, heading=270)
    skeleton2 = ObjectFeedData(ObjectType().Pedestrian, speed=2, heading=270)

    injects = []
    injects += [builder.generate(1, skeleton)]
    injects += [builder.generate(2, skeleton2, 0, 0.0005)]
    injects += [builder.generate(3, skeleton, 0, 0.0010)]
    injects += [builder.generate(4, skeleton2, 0, 0.0015)]
    injects += [builder.generate(5, skeleton, 0, 0.0020)]
    injects += [builder.generate(6, skeleton2, 0, 0.0025)]

    for data in zip(*injects):
        time.sleep(config.timestep)
        for d in data:
            inject(config, d)

# west: 60.163277, 24.914960
# east: 60.163337, 24.915767
