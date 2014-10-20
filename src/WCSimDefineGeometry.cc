#include "WCSimDetectorConstruction.hh"

#include "WCSimPMTConfig.hh"
#include "WCSimPMTManager.hh"

void WCSimDetectorConstruction::SetSuperKGeometry()
{
  ResetPMTConfigs();
//  WCPMTName             ="20inch";
//  WCPMTRadius           =.254*m;  
//  WCPMTExposeHeight     =.18*m; 
	fPMTConfigs.push_back(fPMTManager->GetPMTByName("20inch"));
  WCIDDiameter          = 33.6815*m; //16.900*2*cos(2*pi*rad/75)*m; //inner detector diameter
  WCIDHeight            = 36.200*m; //"" "" height
  WCBarrelPMTOffset     = 0.0715*m; //offset from vertical
  WCBarrelNumPMTHorizontal  = 150; 
  WCBarrelNRings        = 17.;
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3; 
  WCCapPMTSpacing       = 0.707*m; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = 16.9*m;
//  WCPMTGlassThickness   = .4*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_100kton_10inch_40perCent()
{
//  WCPMTName             = "10inch";
//  WCPMTRadius           = .127*m;
//  WCPMTExposeHeight	    = WCPMTRadius - 0.01*m;
  ResetPMTConfigs();
	fPMTConfigs.push_back(fPMTManager->GetPMTByName("10inch"));
  WCIDDiameter          = 53.0*m;
  WCIDHeight            = 60.0*m;
//  WCBarrelPMTOffset	    = WCPMTRadius;
  WCBarrelPMTOffset	    = fPMTConfigs[0].GetRadius();
//	WCBarrelPMTOffset = fPMTManager->GetPMTByName(fPMTNames[0]).GetRadius();
  WCPMTperCellHorizontal = 4.0;
  WCPMTperCellVertical	 = 3.0;
  WCPMTPercentCoverage	 = 40.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/
                                    (10.0*fPMTConfigs[0].GetRadius()));
  WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-
                                    2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/
                                    WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - fPMTConfigs[0].GetRadius();
//  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_100kton_10inch_HQE_12perCent()
{
//  WCPMTName             = "10inchHQE";
//  WCPMTRadius           = .127*m;
//  WCPMTExposeHeight	    = WCPMTRadius - 0.01*m;
  ResetPMTConfigs();
	fPMTConfigs.push_back(fPMTManager->GetPMTByName("10inch_HQE"));
  WCIDDiameter          = 53.0*m;
  WCIDHeight            = 60.0*m;
//  WCBarrelPMTOffset	    = WCPMTRadius;
  WCBarrelPMTOffset	    = fPMTConfigs[0].GetRadius();
  WCPMTperCellHorizontal = 4.0;
  WCPMTperCellVertical	 = 3.0;
  WCPMTPercentCoverage	 = 12.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/
                                    (10.0*fPMTConfigs[0].GetRadius()));
  WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-
                                    2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/
                                    WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - fPMTConfigs[0].GetRadius();
//  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_100kton_10inch_HQE_30perCent()
{
//  WCPMTName             = "10inchHQE";
//  WCPMTRadius           = .127*m;
//  WCPMTExposeHeight	    = WCPMTRadius - 0.01*m;
  ResetPMTConfigs();
  fPMTConfigs.push_back(fPMTManager->GetPMTByName("10inch_HQE"));
  WCIDDiameter          = 53.0*m;
  WCIDHeight            = 60.0*m;
//  WCBarrelPMTOffset	    = WCPMTRadius;
  WCBarrelPMTOffset	    = fPMTConfigs[0].GetRadius();
  WCPMTperCellHorizontal = 4.0;
  WCPMTperCellVertical	 = 3.0;
  WCPMTPercentCoverage	 = 30.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/
                                    (10.0*fPMTConfigs[0].GetRadius()));
  WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-
                                    2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/
                                    WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - fPMTConfigs[0].GetRadius();
//  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_100kton_10inch_HQE_30perCent_Gd()
{
//  WCPMTName             = "10inchHQE";
//  WCPMTRadius           = .127*m;
//  WCPMTExposeHeight	    = WCPMTRadius - 0.01*m;
  ResetPMTConfigs();
  fPMTConfigs.push_back(fPMTManager->GetPMTByName("10inch_HQE"));
  WCIDDiameter          = 53.0*m;
  WCIDHeight            = 60.0*m;
//  WCBarrelPMTOffset	    = WCPMTRadius;
  WCBarrelPMTOffset	    = fPMTConfigs[0].GetRadius();
  WCPMTperCellHorizontal = 4.0;
  WCPMTperCellVertical	 = 3.0;
  WCPMTPercentCoverage	 = 30.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/
                                    (10.0*fPMTConfigs[0].GetRadius()));
  WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-
                                    2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/
                                    WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - fPMTConfigs[0].GetRadius();
//  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = true;
}

void WCSimDetectorConstruction::DUSEL_150kton_10inch_HQE_30perCent()
{
//  WCPMTName             = "10inchHQE";
//  WCPMTRadius           = .127*m;
//  WCPMTExposeHeight	    = WCPMTRadius - 0.01*m;
  ResetPMTConfigs();
  fPMTConfigs.push_back(fPMTManager->GetPMTByName("10inch_HQE"));
  WCIDDiameter          = 64.0*m;
  WCIDHeight            = 60.0*m;
//  WCBarrelPMTOffset	    = WCPMTRadius;
  WCBarrelPMTOffset	    = fPMTConfigs[0].GetRadius();
  WCPMTperCellHorizontal = 4.0;
  WCPMTperCellVertical	 = 3.0;
  WCPMTPercentCoverage	 = 30.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/
                                    (10.0*fPMTConfigs[0].GetRadius()));
  WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-
                                    2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/
                                    WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - fPMTConfigs[0].GetRadius();
//  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_200kton_10inch_HQE_12perCent()
{
//	WCPMTName             = "10inchHQE";
//	WCPMTRadius           = .127*m;
//	WCPMTExposeHeight	    = WCPMTRadius - 0.01*m;
  ResetPMTConfigs();
  fPMTConfigs.push_back(fPMTManager->GetPMTByName("10inch_HQE"));
	WCIDDiameter          = 62.21*m;
	WCIDHeight            = 79.96*m;
//	WCBarrelPMTOffset	    = WCPMTRadius;
  WCBarrelPMTOffset	    = fPMTConfigs[0].GetRadius();
	WCPMTperCellHorizontal = 4.0;
	WCPMTperCellVertical	 = 3.0;
	WCPMTPercentCoverage	 = 12.0;
	WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/
									 (10.0*fPMTConfigs[0].GetRadius()));
	WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-
															   2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/
								   WCPMTperCellVertical));
	WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
	WCCapEdgeLimit        = WCIDDiameter/2.0 - fPMTConfigs[0].GetRadius();
//	WCPMTGlassThickness   = .55*cm;
	WCBlackSheetThickness = 2.0*cm;
	WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_200kton_12inch_HQE_10perCent()
{
//	WCPMTName             = "10inchHQE"; //still using QE from 10 inch tube
//	WCPMTRadius           = .1524*m;
//	WCPMTExposeHeight	    = .118*m;
  ResetPMTConfigs();
  fPMTConfigs.push_back(fPMTManager->GetPMTByName("12inchHQE"));
	WCIDDiameter          = 63.30*m;
	WCIDHeight            = 76.60*m;
	WCBarrelPMTOffset	    = .1537*m;
	WCPMTperCellHorizontal = 1.0;
	WCPMTperCellVertical	 = 1.0;
	WCBarrelNumPMTHorizontal = 232;
	WCBarrelNRings        = 89;
	WCCapPMTSpacing       = .8572*m;
	WCCapEdgeLimit        = 31.424*m;
//	WCPMTGlassThickness   = .55*cm; //guess
	WCBlackSheetThickness = 2.0*cm; //excess, should be just as light-tight
	WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_200kton_12inch_HQE_14perCent()
{
//	WCPMTName             = "10inchHQE"; //still using QE from 10 inch tube
//	WCPMTRadius           = .1524*m;
//	WCPMTExposeHeight	    = .118*m;
  ResetPMTConfigs();
  fPMTConfigs.push_back(fPMTManager->GetPMTByName("12inchHQE"));
	WCIDDiameter          = 63.30*m;
	WCIDHeight            = 76.60*m;
	WCBarrelPMTOffset	    = .1951*m;
	WCPMTperCellHorizontal = 1.0;
	WCPMTperCellVertical	 = 1.0;
	WCBarrelNumPMTHorizontal = 274;
	WCBarrelNRings        = 105;
	WCCapPMTSpacing       = .7258*m;
	WCCapEdgeLimit        = 31.624*m;
//	WCPMTGlassThickness   = .55*cm; //guess
	WCBlackSheetThickness = 2.0*cm; //excess, should be just as light-tight
	WCAddGd               = false;
}

void WCSimDetectorConstruction::CHIPS_25kton_10inch_HQE_10perCent()
{
//  WCPMTName             = "10inchHQE";
//  WCPMTRadius           = .127*m;
//  WCPMTExposeHeight	    = WCPMTRadius - 0.01*m;
  ResetPMTConfigs();
  fPMTConfigs.push_back(fPMTManager->GetPMTByName("10inch_HQE"));
  fPMTConfigs.push_back(fPMTManager->GetPMTByName("10inch"));

  std::cout << "=== PMT LOG ===" << std::endl;
  std::cout << "PMT Vector Size = " << fPMTConfigs.size() << std::endl;
  fPMTConfigs[0].Print();

  WCIDDiameter          = 40.0*m;
  WCIDHeight            = 20.0*m;
  //  WCBarrelPMTOffset	    = WCPMTRadius;
  WCBarrelPMTOffset	    = fPMTConfigs[0].GetRadius();
  WCPMTperCellHorizontal = 4.0;
  WCPMTperCellVertical	 = 3.0;
  WCPMTPercentCoverage	 = 10.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/
                                    (10.0*fPMTConfigs[0].GetRadius()));
  WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-
                                    2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/
                                    WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - fPMTConfigs[0].GetRadius();
//  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::CHIPS_10kton_10inch_HQE_10perCent()
{
//  WCPMTName             = "10inchHQE";
//  WCPMTRadius           = .127*m;
//  WCPMTExposeHeight	    = WCPMTRadius - 0.01*m;
  fPMTConfigs.push_back(fPMTManager->GetPMTByName("10inch_HQE"));

	std::cout << "=== PMT LOG ===" << std::endl;
	std::cout << "PMT Vector Size = " << fPMTConfigs.size() << std::endl;
	fPMTConfigs[0].Print();

  WCIDDiameter          = 25.0*m;
  WCIDHeight            = 20.0*m;
//  WCBarrelPMTOffset	    = WCPMTRadius;
  WCBarrelPMTOffset	    = fPMTConfigs[0].GetRadius();
  WCPMTperCellHorizontal = 4.0;
  WCPMTperCellVertical	 = 3.0;
  WCPMTPercentCoverage	 = 10.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/
                                    (10.0*fPMTConfigs[0].GetRadius()));
  WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-
                                    2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/
                                    WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - fPMTConfigs[0].GetRadius();
//  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::GiantPhotonTest()
{
//  WCPMTName             = "10inchHQE";
//  WCPMTRadius           = .127*m;
//  WCPMTExposeHeight	    = WCPMTRadius - 0.01*m;
  ResetPMTConfigs();
  fPMTConfigs.push_back(fPMTManager->GetPMTByName("10inch_HQE"));

  WCIDDiameter          = 2000.0*m;
  WCIDHeight            = 2000.0*m;
//  WCBarrelPMTOffset	    = WCPMTRadius;
  WCBarrelPMTOffset	    = fPMTConfigs[0].GetRadius();
  WCPMTperCellHorizontal = 4.0;
  WCPMTperCellVertical	 = 3.0;
  WCPMTPercentCoverage	 = 0.0001;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/
                                    (10.0*fPMTConfigs[0].GetRadius()));
  WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-
                                    2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/
                                    WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - fPMTConfigs[0].GetRadius();
//  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

//aah

void WCSimDetectorConstruction::SetMailBox100kTGeometry()  // This should setup a 100kT (metric) Fiducial Volume
{
	// PMT stuff
	this->SetIsMailbox(true);
//    WCPMTName               = "10inch";//normal QE 10 inch tube
//	WCPMTRadius				= .127*m; 
//	WCPMTExposeHeight		= WCPMTRadius - 0.01*m;
  ResetPMTConfigs();
  fPMTConfigs.push_back(fPMTManager->GetPMTByName("10inch"));
//	WCPMTGlassThickness		= .55*cm;
	WCPMTPercentCoverage	= 10;	//% coverage
	WCBlackSheetThickness	= 2.0*cm;
	//Tank and Cavern Dimensions
	WC_MB_Tank_Airgap		=2.0*m;	//Arbitrary here, but this is just the height of air between the top of the outer Veto_thickness and the beginning of the Cavern Dome
	WC_MB_Dome_Height_fraction		=0.2;	//fraction should be <=0.5 
	WC_MB_Veto_Thickness		=2.0*m;	// This is just the spacing (in water) between the outer side of the Blacksheet and the beginning of the fiducial volume
	WC_MB_Buffer_Thickness	=2.5*m; //Distance between wall and beginning of the Fiducial volume
	//Fiducial VOlume Stuff--these numbers are taken from table 9.1 of DUSEL LCAB Report #2, 7/2009, adjusted to Fiducial Volume (F.V.) and 2.5m gap between wall and F.V.
	WC_MB_Fid_Length          = (45.3333*m+2*WC_MB_Veto_Thickness);// strictly speaking this isn't the fiducial length anymore, but increased by the veto thickness
	WC_MB_Fid_Width           = (40.0*m+2*WC_MB_Veto_Thickness);
	WC_MB_Fid_Depth           = (55.0*m+2*WC_MB_Veto_Thickness);	// how deep is the bottom layer of tubes at. Tank depth is a little more.
	//	WC_MB_Fid_Length          = 1.0*m+2*WC_MB_Veto_Thickness;// tiny test detector!
	//	WC_MB_Fid_Width           = 1.000*m+2*WC_MB_Veto_Thickness;// tiny test detector!
	//	WC_MB_Fid_Depth           = 1.0*m+2*WC_MB_Veto_Thickness;// tiny test detector!
    WCAddGd               = false;
}

// Leigh: Let's add a couple more mailbox geometries. The 100kT is fine, just need some other PMT coverages
// Just call the other constructor and override the PMT coverage :)
void WCSimDetectorConstruction::SetMailBox100kTGeometry_20perCent(){
	this->SetMailBox100kTGeometry();
	WCPMTPercentCoverage = 20;
}
void WCSimDetectorConstruction::SetMailBox100kTGeometry_30perCent(){
	this->SetMailBox100kTGeometry();
	WCPMTPercentCoverage = 30;
}
void WCSimDetectorConstruction::SetMailBox100kTGeometry_40perCent(){
	this->SetMailBox100kTGeometry();
	WCPMTPercentCoverage = 40;
}
void WCSimDetectorConstruction::SetMailBox100kTGeometry_50perCent(){
	this->SetMailBox100kTGeometry();
	WCPMTPercentCoverage = 50;
}

void WCSimDetectorConstruction::SetMailBox100x20x30Geometry()  // This should setup a 60kT-ish (metric) Fiducial Volume
{
	// PMT stuff
	this->SetIsMailbox(true);
//  WCPMTName               = "10inch";//normal QE 10 inch tube
//	WCPMTRadius				= .127*m; 
//	WCPMTExposeHeight		= WCPMTRadius - 0.01*m;
  ResetPMTConfigs();
  fPMTConfigs.push_back(fPMTManager->GetPMTByName("10inch"));
//	WCPMTGlassThickness		= .55*cm;
	WCPMTPercentCoverage	= 10;	//% coverage
	WCBlackSheetThickness	= 2.0*cm;
	//Tank and Cavern Dimensions
	WC_MB_Tank_Airgap		=2.0*m;	//Arbitrary here, but this is just the height of air between the top of the outer Veto_thickness and the beginning of the Cavern Dome
	WC_MB_Dome_Height_fraction		=0.2;	//fraction should be <=0.5 
	WC_MB_Veto_Thickness		=2.0*m;	// This is just the spacing (in water) between the outer side of the Blacksheet and the beginning of the fiducial volume
	WC_MB_Buffer_Thickness	=2.5*m; //Distance between wall and beginning of the Fiducial volume
	//Fiducial VOlume Stuff--these numbers are taken from table 9.1 of DUSEL LCAB Report #2, 7/2009, adjusted to Fiducial Volume (F.V.) and 2.5m gap between wall and F.V.
	WC_MB_Fid_Length          = (100.0*m+2*WC_MB_Veto_Thickness);// strictly speaking this isn't the fiducial length anymore, but increased by the veto thickness
	WC_MB_Fid_Width           = (20.0*m+2*WC_MB_Veto_Thickness);
	WC_MB_Fid_Depth           = (30.0*m+2*WC_MB_Veto_Thickness);	// how deep is the bottom layer of tubes at. Tank depth is a little more.
	//	WC_MB_Fid_Length          = 1.0*m+2*WC_MB_Veto_Thickness;// tiny test detector!
	//	WC_MB_Fid_Width           = 1.000*m+2*WC_MB_Veto_Thickness;// tiny test detector!
	//	WC_MB_Fid_Depth           = 1.0*m+2*WC_MB_Veto_Thickness;// tiny test detector!
    WCAddGd               = false;
}
// End Leigh's new mailbox detectors

void WCSimDetectorConstruction::SetMailBox150kTGeometry_10inch_HQE_30perCent()  // This should setup a 150kT (metric) Fiducial Volume
{
	// PMT stuff
//    WCPMTName               = "10inchHQE";//high qe 10 " tube
//	WCPMTRadius				= .127*m; 
//	WCPMTExposeHeight		= WCPMTRadius - 0.01*m;
  ResetPMTConfigs();
  fPMTConfigs.push_back(fPMTManager->GetPMTByName("10inch_HQE"));
//	WCPMTGlassThickness		= .55*cm;
	WCPMTPercentCoverage	= 30;	//% coverage
	WCBlackSheetThickness	= 2.0*cm;
	//Tank and Cavern Dimensions
	WC_MB_Tank_Airgap		=2.0*m;	//Arbitrary here, but this is just the height of air between the top of the outer Veto_thickness and the beginning of the Cavern Dome
	WC_MB_Dome_Height_fraction	=0.2;//fraction should be <=0.5
	WC_MB_Veto_Thickness	=2.0*m;// This is just the spacing (in water) between the outer side of the Blacksheet and the beginning of the fiducial volume
	WC_MB_Buffer_Thickness	=2.5*m; //Distance between wall and beginning of the Fiducial volume
	//Fiducial VOlume Stuff--these numbers are taken from table 9.1 of DUSEL LCAB Report #2, 7/2009, adjusted to Fiducial Volume (F.V.) and 2.5m gap between wall and F.V.
	WC_MB_Fid_Length		= (68.0*m+2*WC_MB_Veto_Thickness);	// strictly speaking this isn't the fiducial length anymore, but increased by the veto thickness
	WC_MB_Fid_Width			= (40.0*m+2*WC_MB_Veto_Thickness);
	WC_MB_Fid_Depth         = (55.0*m+2*WC_MB_Veto_Thickness);	// how deep is the bottom layer of tubes at. Tank depth is a little more.
    WCAddGd               = false;
}

void WCSimDetectorConstruction::SetMailBox150kTGeometry_10inch_40perCent()  // This should setup a 150kT (metric) Fiducial Volume
{
	// PMT stuff
//    WCPMTName               = "10inch";//normal qe 10 inch tube
//	WCPMTRadius				= .127*m; 
//	WCPMTExposeHeight		= WCPMTRadius - 0.01*m;
  ResetPMTConfigs();
  fPMTConfigs.push_back(fPMTManager->GetPMTByName("10inch"));
//	WCPMTGlassThickness		= .55*cm;
	WCPMTPercentCoverage	= 40;	//% coverage
	WCBlackSheetThickness	= 2.0*cm;
	//Tank and Cavern Dimensions
	WC_MB_Tank_Airgap		=2.0*m;	//Arbitrary here, but this is just the height of air between the top of the outer Veto_thickness and the beginning of the Cavern Dome
	WC_MB_Dome_Height_fraction	=0.2;//fraction should be <=0.5
	WC_MB_Veto_Thickness	=2.0*m;// This is just the spacing (in water) between the outer side of the Blacksheet and the beginning of the fiducial volume
	WC_MB_Buffer_Thickness	=2.5*m; //Distance between wall and beginning of the Fiducial volume
	//Fiducial VOlume Stuff--these numbers are taken from table 9.1 of DUSEL LCAB Report #2, 7/2009, adjusted to Fiducial Volume (F.V.) and 2.5m gap between wall and F.V.
	WC_MB_Fid_Length		= (68.0*m+2*WC_MB_Veto_Thickness);	// strictly speaking this isn't the fiducial length anymore, but increased by the veto thickness
	WC_MB_Fid_Width			= (40.0*m+2*WC_MB_Veto_Thickness);
	WC_MB_Fid_Depth         = (55.0*m+2*WC_MB_Veto_Thickness);	// how deep is the bottom layer of tubes at. Tank depth is a little more.
	WCAddGd               = false;
}

void WCSimDetectorConstruction::SetMailBox300kTGeometry()    // This should setup a 300kT (metric) Fiducial Volume
{
	// PMT stuff
//    WCPMTName               = "10inch";//normal qe 10 inch tube
//	WCPMTRadius				=.127*m;
//	WCPMTExposeHeight		=WCPMTRadius - 0.01*m;
  ResetPMTConfigs();
  fPMTConfigs.push_back(fPMTManager->GetPMTByName("10inch"));
//	WCPMTGlassThickness		= .55*cm;
	WCPMTPercentCoverage	= 10;	//% coverage
	WCBlackSheetThickness	= 2.0*cm;
	//Tank and Cavern Dimensions
	WC_MB_Tank_Airgap		=2.0*m;	//Arbitrary here, but this is just the height of air between the top of the outer Veto_thickness and the beginning of the Cavern Dome
	WC_MB_Dome_Height_fraction	=0.2;//fraction should be <=0.5
	WC_MB_Veto_Thickness	=2.0*m;// This is just the spacing (in water) between the outer side of the Blacksheet and the beginning of the fiducial volume
	WC_MB_Buffer_Thickness	=2.5*m; //Distance between wall and beginning of the Fiducial volume
	//Fiducial VOlume Stuff--these numbers are taken from table 9.1 of DUSEL LCAB Report #2, 7/2009, adjusted to Fiducial Volume (F.V.) and 2.5m gap between wall and F.V.
	WC_MB_Fid_Length        =( 136.0*m+2*WC_MB_Veto_Thickness);	//	// strictly speaking this isn't the fiducial length anymore, but increased by the veto thickness
	WC_MB_Fid_Width         = (40.0*m+2*WC_MB_Veto_Thickness);	// this gives a volume of 299.2 metric tons
	WC_MB_Fid_Depth         = (55.0*m+2*WC_MB_Veto_Thickness);	// how deep is the bottom layer of tubes at. Tank depth is a little more.
    WCAddGd               = false;
}
//aah

