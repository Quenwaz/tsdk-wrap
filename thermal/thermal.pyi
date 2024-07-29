from numpy.typing import DTypeLike
from numpy import int8
from enum import Enum

class PseudoColorType(Enum):
    # 白热
    PSEUDO_COLOR_WHITEHOT = 0
    # 岩溶
    PSEUDO_COLOR_FULGURITE = 1
    # 铁红
    PSEUDO_COLOR_IRONRED = 2
    # 热铁
    PSEUDO_COLOR_HOTIRON = 3
    # 医疗
    PSEUDO_COLOR_MEDICAL = 4
    # 北极
    PSEUDO_COLOR_ARCTIC = 5
    # 彩虹1
    PSEUDO_COLOR_RAINBOW1 = 6
    # 彩虹2
    PSEUDO_COLOR_RAINBOW2 = 7
    # 描红
    PSEUDO_COLOR_TINT = 8
    # 黑热
    PSEUDO_COLOR_BLACKHOT = 9

class ThermalInfo:
    """红外信息类
    """

    def __init__(self) -> None:
        ...
        
    def open(self, rjpg:str) -> bool:
        """实例化

        Args:
            rjpg (str): 红外图片路径
        Returns:
            bool: 是否打开
        """
        ...
        
    def close(self) -> None:
        """关闭当前打开的资源， 请务必在使用完调用
        """
        ...
        
    def enable_log(self, logfile:str)->None:
        """启用调试日志

        Args:
            logfile (str): 日志路径
        """
        ...

    def set_measurement_params(self,humidity:float, emissivity:float, reflection:float, ambient_temp:float, distance:float)->bool: 
        """设置测温参数

        Args:
            humidity (float): 湿度
            emissivity (float): 发射率
            reflection (float): 反射率
            ambient_temp (float): 环境温度
            distance (float): 测温距离

        Returns:
            bool: 是否设置成功
        """
        ...

    def get_measurement_params(self)->dict: 
        """获取当前测温参数

        Returns:
            dict: 返回字典
        """
        ...


    def get_temperature(self)->DTypeLike[float]: 
        """获取图片每像素温度

        Returns:
            DTypeLike[float]: height x width
        """
        ...


    def set_pseudo_color(self, type:PseudoColorType)->bool: 
        """设置伪彩色

        Args:
            type (PseudoColorType): 伪彩色类型

        Returns:
            bool: 是否设置成功
        """
        ...

    def get_pseudo_color()->PseudoColorType:
        """获取当前图片伪彩色类型

        Returns:
            PseudoColorType: 类型
        """
        ...

    def get_raw_image(self)->DTypeLike[int8]: 
        """获取当前图像

        Returns:
            DTypeLike[int8]: height x width x 3
        """
        ...

