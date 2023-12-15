from .pymidsx import InteractionData as _InteractionData
import numpy as np


class InteractionData(_InteractionData):
    def __init__(self, materials, db_path):
        super().__init__(materials, db_path)

    def interpolate_max_total_cross_section(self, values):
        if np.isscalar(values):  # If the input is a scalar
            return super().interpolate_max_total_cross_section(values)
        else:  # If the input is an array
            vfunc = np.vectorize(super().interpolate_max_total_cross_section)
            return vfunc(values)