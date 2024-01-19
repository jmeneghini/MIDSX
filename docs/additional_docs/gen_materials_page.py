import sqlite3
import pandas as pd
import os

# Change working directory to the directory of this file
os.chdir(os.path.dirname(os.path.abspath(__file__)))

db_path = '../../data/data_sources/EPDL/midsx.db'
conn = sqlite3.connect(db_path)
c = conn.cursor()

# Export Materials table to pandas dataframe
df = pd.read_sql_query("SELECT * FROM Materials", conn)

# Add units to density column name
df.rename(columns={'Density': 'Density (g/cm3)'}, inplace=True)

# Export df to markdown
df.to_markdown('Materials.md', index=False)

header = "# Materials Available For Simulation"

# Add header to markdown file
with open('Materials.md', 'r+') as f:
    content = f.read()
    f.seek(0, 0)
    f.write(header + '\n' + content)

