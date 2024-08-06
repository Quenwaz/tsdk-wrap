import sys
import ctypes
import numpy as np
import enum

__all__ = ["PseudoColorType", "ThermalInfo"]

class PseudoColorType(enum.Enum):
    DIRP_PSEUDO_COLOR_WHITEHOT = 0  #             /**<  0: White Hot */
    DIRP_PSEUDO_COLOR_FULGURITE = 1  #               /**<  1: Fulgurite */
    DIRP_PSEUDO_COLOR_IRONRED =2  #              /**<  2: Iron Red */
    DIRP_PSEUDO_COLOR_HOTIRON = 3  #               /**<  3: Hot Iron */
    DIRP_PSEUDO_COLOR_MEDICAL = 4  #                /**<  4: Medical */
    DIRP_PSEUDO_COLOR_ARCTIC = 5  #                /**<  5: Arctic */
    DIRP_PSEUDO_COLOR_RAINBOW1 = 6  #               /**<  6: Rainbow 1 */
    DIRP_PSEUDO_COLOR_RAINBOW2 = 7 #                /**<  7: Rainbow 2 */
    DIRP_PSEUDO_COLOR_TINT = 8 #               /**<  8: Tint */
    DIRP_PSEUDO_COLOR_BLACKHOT = 9  #                 /**<  9: Black Hot */
    
    
def Singleton(cls):
    _instance = {}

    def _singleton(*args, **kargs):
        if cls not in _instance:
            _instance[cls] = cls(*args, **kargs)
        return _instance[cls]

    return _singleton

@Singleton
class ThermalInfo:

    def __init__(self, verbose=False):
        import os
        dll_path = os.path.join(os.path.abspath(os.path.dirname(__file__)), "libs", "thermal.dll")
        # 设置工作路径， 否则加载失败
        oldcwd = os.getcwd()
        os.chdir(os.path.dirname(dll_path))
        self.verbose = verbose
        self.thermal = ctypes.cdll.LoadLibrary(dll_path)
        os.chdir(oldcwd)
        self.handle:ctypes.c_void_p = ctypes.c_void_p(None)


    def open(self, rjpgpath: str):
        """打开图片

        Args:
            rjpgpath (str): rjpg路径

        Returns:
            void*: 返回句柄
        """
        self.__verbose()
        self.thermal.open.restype = ctypes.c_void_p
        rpath = ctypes.c_char_p(rjpgpath.encode("utf-8"))
        self.handle  = ctypes.c_void_p(self.thermal.open(rpath))
        if self.handle.value == None:
            raise Exception(f"open {rjpgpath} failed.")

        return True
    
    def close(self):
        """关闭当前打开的图片

        """
        self.__verbose()
        if self.handle.value != None:
            self.thermal.close(self.handle)
            
        self.handle = ctypes.c_void_p(None)

    def set_measurement_params(self,humidity:float, emissivity:float, reflection:float, ambient_temp:float, distance:float)->bool:
        """设置测温参数

        Args:
            humidity (float): 湿度
            emissivity (float): 辐射度
            reflection (float): 反射度
            ambient_temp (float): 环境温度
            distance (float): 测温距离

        Returns:
            bool: 是否设置成功
        """
        self.__verbose()
        self.thermal.set_measurement_params.restype = ctypes.c_bool
        return self.thermal.set_measurement_params(self.handle, 
                                                   ctypes.c_float(humidity), 
                                                   ctypes.c_float(emissivity), 
                                                   ctypes.c_float(reflection), 
                                                   ctypes.c_float(ambient_temp), 
                                                   ctypes.c_float(distance))

    def get_measurement_params(self):
        """获取当前测温参数

        Returns:
            dict: 返回参数字典
        """
        self.__verbose()
        humidity = ctypes.c_float()
        emissivity = ctypes.c_float()
        reflection = ctypes.c_float()
        ambient_temp = ctypes.c_float()
        distance = ctypes.c_float()
        self.thermal.get_measurement_params.restype = ctypes.c_bool
        ret:ctypes.c_bool = self.thermal.get_measurement_params(
            self.handle, 
            ctypes.byref(humidity), 
            ctypes.byref(emissivity), 
            ctypes.byref(reflection), 
            ctypes.byref(ambient_temp), 
            ctypes.byref(distance))
        
        if ret:
            return {
                "humidity": humidity.value,
                "emissivity": emissivity.value,
                "reflection": reflection.value,
                "ambient_temp": ambient_temp.value,
                "distance": distance.value,
            }
        

    def get_temperature(self):
        """获取图片温度

        Returns:
            numpy.array: numpy数组
        """
        self.__verbose()
        self.thermal.get_temperature.restype = ctypes.POINTER(ctypes.c_float)
        row = ctypes.c_int()
        col = ctypes.c_int()
        ret = self.thermal.get_temperature(self.handle, ctypes.byref(row), ctypes.byref(col))
        # np.ctypeslib.as_array 将与ret共享内存， 所以需要深拷贝
        temperature = np.ctypeslib.as_array(ret,(row.value, col.value)).copy()
        self.thermal.free_temperature(ret)
        return temperature

    def get_raw_image(self):
        """获取原始图像

        Returns:
            numpy.array: numpy数组
        """
        self.__verbose()
        row = ctypes.c_int()
        col = ctypes.c_int()
        self.thermal.get_raw_image.restype = ctypes.POINTER(ctypes.c_ubyte)
        ret = self.thermal.get_raw_image(self.handle, ctypes.byref(row), ctypes.byref(col))
        # np.ctypeslib.as_array 将与ret共享内存， 所以需要深拷贝
        raw_image = np.ctypeslib.as_array(ret,(row.value, col.value, 3)).copy()
        self.thermal.free_raw_image(ret)
        return raw_image


    def set_pseudo_color(self, pseudo_type:PseudoColorType) -> bool:
        """设置图片伪彩色

        Args:
            pseudo_type (PseudoColorType): 伪彩色类型

        Returns:
            bool: 是否设置成功
        """
        self.__verbose()
        self.thermal.set_pseudo_color.restype = ctypes.c_bool
        return self.thermal.set_pseudo_color(self.handle, pseudo_type.value)

    def get_pseudo_color(self) -> PseudoColorType:
        """获取当前伪彩色

        Returns:
            PseudoColorType: 伪彩色类型
        """
        self.__verbose()
        self.thermal.get_pseudo_color.restype = ctypes.c_int
        return PseudoColorType(self.thermal.get_pseudo_color(self.handle))
    
    def __verbose(self):
        if self.verbose:
            print("调用接口 {}".format(sys._getframe(1).f_code.co_name))


if __name__ == "__main__":
    thermal = ThermalInfo(True)
    thermal.open(r"0B2D25BC332B40D09D8E6DD60050B00A.jpg")
    temperature = thermal.get_temperature()
    raw_image = thermal.get_raw_image()
    thermal.close()
