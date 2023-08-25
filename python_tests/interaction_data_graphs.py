import sys
# Remove the path to the python folder. This is necessary to import the C++ module from site-packages
# sys.path.remove("/home/john/Documents/MCXrayTransport/python")

import pymidsx
from pymidsx import quantity

print(dir(pymidsx))

db_path = "/home/john/Documents/MCXrayTransport/data/data_sources/EPDL/EPDL.db"

voxel_grid_path = "/home/john/Documents/MCXrayTransport/data/voxels/al_hvl_30keV.nii.gz"
voxel_grid = pymidsx.VoxelGrid(voxel_grid_path)

quantity_list = [quantity.NumberOfParticles(), quantity.NumberOfInteractions()]

quantity_container = quantity.QuantityContainer(quantity_list)

material_list = ["Al"]
interaction_data_wrapper = pymidsx.InteractionData(material_list, db_path)

disc_tally = pymidsx.DiscSurfaceTally([2, 2, 0], [0, 0, 1], 1, quantity_container)
print(dir(disc_tally))

physics_engine = pymidsx.PhysicsEngine(voxel_grid, interaction_data_wrapper, [disc_tally])

directionality = pymidsx.BeamDirectionality([0, 0, 1])
spectrum = pymidsx.MonoenergeticSpectrum(30E3)
geometry = pymidsx.PointGeometry([2, 2, 0])
source = pymidsx.PhotonSource(spectrum, directionality, geometry)

pymidsx.run_simulation(source, physics_engine, 1000000)




