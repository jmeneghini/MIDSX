import sqlite3
import pandas as pd

db_path = './data/data_sources/EPDL/EPDL.db'
conn = sqlite3.connect(db_path)
c = conn.cursor()

# Export Materials table to pandas dataframe
df = pd.read_sql_query("SELECT * FROM Materials", conn)

# Add units to density column name
df.rename(columns={'Density': 'Density (g/cm3)'}, inplace=True)

# Export df to html
df.to_html('./docs/html/materials.html', index=False)

