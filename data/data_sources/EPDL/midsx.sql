CREATE TABLE IF NOT EXISTS "Elements" (
	"ID" BIGINT, 
	"Symbol" TEXT, 
	"AtomicNumber" BIGINT, 
	"Mass" FLOAT, 
	"Density" TEXT
);
CREATE TABLE IF NOT EXISTS "TotalCrossSections" (
	"index" BIGINT, 
	"ElementID" BIGINT, 
	"Energy" FLOAT, 
	"CrossSection" FLOAT
);
CREATE INDEX "ix_TotalCrossSections_index" ON "TotalCrossSections" ("index");
CREATE TABLE IF NOT EXISTS "CoherentScatteringCrossSections" (
	"index" BIGINT, 
	"ElementID" BIGINT, 
	"Energy" FLOAT, 
	"CrossSection" FLOAT
);
CREATE INDEX "ix_CoherentScatteringCrossSections_index" ON "CoherentScatteringCrossSections" ("index");
CREATE TABLE IF NOT EXISTS "IncoherentScatteringCrossSections" (
	"index" BIGINT, 
	"ElementID" BIGINT, 
	"Energy" FLOAT, 
	"CrossSection" FLOAT
);
CREATE INDEX "ix_IncoherentScatteringCrossSections_index" ON "IncoherentScatteringCrossSections" ("index");
CREATE TABLE IF NOT EXISTS "PairProductionCrossSectionsElectronField" (
	"index" BIGINT, 
	"ElementID" BIGINT, 
	"Energy" FLOAT, 
	"CrossSection" FLOAT
);
CREATE INDEX "ix_PairProductionCrossSectionsElectronField_index" ON "PairProductionCrossSectionsElectronField" ("index");
CREATE TABLE IF NOT EXISTS "PairProductionCrossSectionsNuclearField" (
	"index" BIGINT, 
	"ElementID" BIGINT, 
	"Energy" FLOAT, 
	"CrossSection" FLOAT
);
CREATE INDEX "ix_PairProductionCrossSectionsNuclearField_index" ON "PairProductionCrossSectionsNuclearField" ("index");
CREATE TABLE IF NOT EXISTS "TotalPhotoIonizationCrossSections" (
	"index" BIGINT, 
	"ElementID" BIGINT, 
	"Energy" FLOAT, 
	"CrossSection" FLOAT
);
CREATE INDEX "ix_TotalPhotoIonizationCrossSections_index" ON "TotalPhotoIonizationCrossSections" ("index");
CREATE TABLE IF NOT EXISTS "CoherentScatteringFormFactors" (
	"index" BIGINT, 
	"ElementID" BIGINT, 
	"Energy" FLOAT, 
	"FormFactor" FLOAT
);
CREATE INDEX "ix_CoherentScatteringFormFactors_index" ON "CoherentScatteringFormFactors" ("index");
CREATE TABLE IF NOT EXISTS "IncoherentScatteringFunctions" (
	"index" BIGINT, 
	"ElementID" BIGINT, 
	"Energy" FLOAT, 
	"ScatteringFunction" FLOAT
);
CREATE INDEX "ix_IncoherentScatteringFunctions_index" ON "IncoherentScatteringFunctions" ("index");
CREATE TABLE IF NOT EXISTS "PairProductionTotalCrossSections" (
	"index" BIGINT, 
	"ElementID" BIGINT, 
	"Energy" FLOAT, 
	"CrossSection" FLOAT
);
CREATE INDEX "ix_PairProductionTotalCrossSections_index" ON "PairProductionTotalCrossSections" ("index");
CREATE TABLE IF NOT EXISTS "ImaginaryAnomalousScatteringFactors" (
	"index" BIGINT, 
	"ElementID" BIGINT, 
	"Energy" FLOAT, 
	"ScatteringFactor" FLOAT
);
CREATE INDEX "ix_ImaginaryAnomalousScatteringFactors_index" ON "ImaginaryAnomalousScatteringFactors" ("index");
CREATE TABLE IF NOT EXISTS "RealAnomalousScatteringFactors" (
	"index" BIGINT, 
	"ElementID" BIGINT, 
	"Energy" FLOAT, 
	"ScatteringFactor" FLOAT
);
CREATE INDEX "ix_RealAnomalousScatteringFactors_index" ON "RealAnomalousScatteringFactors" ("index");
