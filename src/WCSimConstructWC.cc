#include "WCSimDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4IntersectionSolid.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4PVReplica.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4AssemblyVolume.hh"
#include "G4SubtractionSolid.hh"
#include "globals.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpticalSurface.hh"
#include "G4UserLimits.hh"
#include "G4ReflectionFactory.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"


#include "G4SDManager.hh"
#include "WCSimWCSD.hh"
#include "WCSimPMTParams.hh"
#include "WCSimPMTConfig.hh"
#include "WCSimTuningParameters.hh" //jl145

G4float WCSimDetectorConstruction::GetPMTQE(G4float PhotonWavelength, G4int flag, G4float low_wl, G4float high_wl, G4float ratio){
  // XQ  08/17/10
  // Decide to include the QE in the WCSim detector 
  // rathe than hard coded into the StackingAction
  // This way, one can load this QE in both the StackingAction
  // and the sensitive detector.

  // flag
  // flag == 0 return the maximum QE for all wave length
  // flag == 1 return the actual QE for the wave length


  // low_wl and high_wl 
  // remove any optical photons outside the range
  // basically return QE = 0
  
  // ratio, fudge factor to increase QE for certain purpose

  // return 0 for wavelenght outside the range
  if (flag==1){
    if (PhotonWavelength <= low_wl || PhotonWavelength >= high_wl || PhotonWavelength <=280 || PhotonWavelength >=660){
      return 0;
    }
  }else if (flag==0){
    if (PhotonWavelength <= low_wl || PhotonWavelength >= high_wl){
      return 0;
    }
  }
  
	// Leigh:
	// Get the quantum efficiency as a function of wavelength
	// First number in the pair is the wavelength, second is the efficiency
	std::vector<std::pair<double,double> > qEff = fPMTConfigs[0].GetEfficiency();
	const G4float qeMax = fPMTConfigs[0].GetMaxEfficiency();

	G4double newWave = 0;

	if(flag == 1){
		for(unsigned int i = 0; i < qEff.size() - 1; ++i){
			if(PhotonWavelength <= qEff[i+1].first){
				double wave1 = qEff[i].first;
				double eff1 = qEff[i].second;
				double wave2 = qEff[i+1].first;
				double eff2 = qEff[i+1].second;
				newWave = eff1 + (eff2 - eff1)/(wave2-wave1)*(PhotonWavelength-wave1);
				break;
			}
		}
	}
	else if (flag == 0){
		newWave = qeMax;
	}

  newWave = newWave *ratio;
  
  return newWave;
}

// Geometry definitions for the detectors moved to WCSimDefineGeometry.cc

//PMT logical volume construction is moved to seperate file, WCSimConstructPMT.
//Still method within WCSimDetectorConstruction


//aah
G4LogicalVolume* WCSimDetectorConstruction::ConstructMailboxWC()
{
	debugMode=false;
	//attempt by aah--1/21/10-2/22/10
	
	//Define geometry of world volumes. At this stage, the world volume will just be the cavern of the excavated detector. It is a rectangular parallelepiped, with a circular arc
	//ceiling.
	
	// The size is set by the dimensions of the fiducial volume declared in the set geometry function, with added spacing for veto/dead water regions, and air just above water surface,
	// extending into the cylindrical domed area.
	// The origin (0,0,0) is at the center of the "Fiducial" and "Tank_H2O" volumes. The Tank_H2O volume size includes:
	// 1) the thin PMT volumes that sit just outside the Fiducial Volume's six faces, and
	// 2) a "Veto" region. Whether this Veto is actually a veto, or simply an inactive layer of water which might include the pmt support structures is left open. The "Veto" region is
	//     the same thickness on all six sides.
	//
	// Above the top of the upper "Veto" region, a rectangular "Airgap" is included, to simulate what might be there.
	// Above the "Airgap" is the domed upper part of the cavern. I give a variable, "WC_MB_Dome_Height_fraction" that sets the shape of the Dome. If this variable=0.5, then the dome
	// is just a half cylinder, whose radius is equal to the "WC_MB_Fid_Width" parameter. If the variable is less than 0.5, then the dome is a smaller section of a cylinder, with the
	// chord also equal to the "WC_MB_Fid_Width" setting. You should not choose values >0.5!---that would look pretty odd I think, the dome being a shape which is larger than a halfcylinder.
	
	// The PMT volumes are built as LxW, WxD, and LxD faces. (L=length, W= width, D=Depth). The length is along the "X" axis, the width along the "Y" axis, and depth alond the "Z" axis.
	//Since the PMT as initially constructed is symmetric about the Z axis, and is facing "upward", along the "Z axis, I build each of the 6 faces, initally oriented on the XY plane,
	// (symmetric around the origin). The PMT spacing is determined automatically from the "WCPMTPercentCoverage" value. Currently the surface area of a hemisphere (2*pi*r^2) is used'
	// for the PMT area. The maximum number of PMT's are placed symmetrically (about the origin) in this thin layer. Then the Blacksheet is laid in. The LxW layers are extended over the WxD 
	// and LxD layers to make a cap.Then "strips" of blacksheet are laid in this extended region to "light-seal" the PMT and Fiducial volumes. The WxD face is also extended on the W dimension
	// as to overlap the LxD face, and then strips of blacksheet are added to seal this too. So in the end, no light from outside the PMT volume can get inside and trugger a PMT (I hope!)
	
	// After the faces are constructed (and I also construct a mirror image for the opposite face), the face is rotated (if necessary) and then translated so that the PMT faces are just
	// outside the Fiducial volume and looking inward into the fiducial volume. Both the PMT volume and the Fiducial Volumes are daughter volumes of the Tank_H2O volume.
	
	
	//For the most part, I think I follow the same prescriptions for sensitizing the PMT's, and making the optical surfaces for the glass and blacksheet as was done for the Cylindrical 
	// Detector.
	
	// The Drawing of the detector shows the watertank volume, with the Fiducial volume and PMT volumes inside.
	
	
	WCPosition=0.;//     aah    WCPosition is used as a depth offset in the Cylinder Detector. I set it to "0" here.
//	G4double WC_ActiveLayer_Depth  = WCBlackSheetThickness+WCPMTRadius+0.01*m;             //add 1cm extra thickness just in case
	G4double WC_ActiveLayer_Depth  = WCBlackSheetThickness+fPMTConfigs[0].GetRadius()+0.01*m;             //add 1cm extra thickness just in case
	G4double WC_MB_Cavern_length = WC_MB_Fid_Length+2*(WC_MB_Buffer_Thickness-WC_MB_Veto_Thickness);
	G4double WC_MB_Cavern_width = WC_MB_Fid_Width+2*(WC_MB_Buffer_Thickness-WC_MB_Veto_Thickness);
	G4double WC_MB_Tank_depth = WC_MB_Fid_Depth+2*(WC_MB_Buffer_Thickness-WC_MB_Veto_Thickness);      //note that I want the 0,0,0 position oriented in center of water tank, so I won't include airgap here
	G4cout << "Cavern Length= "<< WC_MB_Cavern_length/m << "m,  Cavern_Width= "<< WC_MB_Cavern_width/m << "m, Tank Depth= "<< WC_MB_Tank_depth/m << "m \n";

	WC_MB_Fid_Length -= 2*(WC_ActiveLayer_Depth-WCBlackSheetThickness);	//these 3 lines are to adjust the fiducial size to "fake out" the sensistive volume size so
	WC_MB_Fid_Width -= 2*(WC_ActiveLayer_Depth-WCBlackSheetThickness);	// that it will fit the FV+"veto" with the blacksheet front face 0.5 m from the wall
	WC_MB_Fid_Depth -= 2*(WC_ActiveLayer_Depth-WCBlackSheetThickness);	// and 2m from the FV. I know it is a complicated mess!
	
	
	// now we know the extent of the detector and are able to tune the tolerance
	G4GeometryManager::GetInstance()->SetWorldMaximumExtent(WC_MB_Fid_Length > WC_MB_Fid_Depth ? WC_MB_Fid_Length : WC_MB_Fid_Depth);
	G4cout << "Computed tolerance = "<< G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm<< " mm" << G4endl;			
	
	// Now make the geometry
	G4double WC_MB_Dome_Height=WC_MB_Dome_Height_fraction*WC_MB_Cavern_width;//
	G4Box* WC_MB_tank_H20= 	new G4Box("Tank",	WC_MB_Cavern_length/2.0	,WC_MB_Cavern_width/2.0,	WC_MB_Tank_depth/2.0);//	Note I will add a rectangular airgap above this.
	G4Box*	WC_MB_Tank_Airgap_volume=	new G4Box("Airgap",	WC_MB_Cavern_length/2.0	,	WC_MB_Cavern_width/2.0,		WC_MB_Tank_Airgap/2.0);	//This is airgap above water
	
	WCLength =(WC_MB_Cavern_length > WC_MB_Tank_depth+WC_MB_Tank_Airgap+ WC_MB_Dome_Height? WC_MB_Cavern_length : WC_MB_Tank_depth+WC_MB_Tank_Airgap+WC_MB_Dome_Height);

    //Decide if adding Gd
    G4String water = "Water";
    if (WCAddGd)
    {water = "Doped Water";}
	
	
	//*********************											  
	//This is for calculation of the ceiling volume
	
	G4double phi_ceiling = 2.*atan(2.*WC_MB_Dome_Height/WC_MB_Cavern_width);
	G4double radius_ceiling=		WC_MB_Cavern_width/(2 *sin(phi_ceiling));                   //so this volume is not necessarily a half cylinder (phi=pi)
	G4cout << "phi" << phi_ceiling <<"   radius= "<< radius_ceiling/m<<"m"<<"\n";
	G4cout << "dome extent" << pi-phi_ceiling <<"    "<< pi+phi_ceiling<<"\n";
	G4Tubs* WC_MB_Cavern_ceiling_cyl = new G4Tubs("ceiling_cyl",0.0,radius_ceiling,WC_MB_Cavern_length/2.,pi-phi_ceiling,2*phi_ceiling);
	G4Box* subtract_to_make_ceiling= new G4Box("subtract",radius_ceiling,radius_ceiling,1+WC_MB_Cavern_length/2.);///a little bit longer since I seemed to have some extra stuff left over.
	G4SubtractionSolid*  WC_MB_Cavern_ceiling= new G4SubtractionSolid("Ceiling",WC_MB_Cavern_ceiling_cyl,subtract_to_make_ceiling,0,G4ThreeVector(+WC_MB_Dome_Height,0.,0.));//this is now ceiling, but it needs to be rotated +90 degrees around y axis
	G4Transform3D rotation_dome = G4RotateY3D(pi/2.);
	G4double delta=-radius_ceiling+WC_MB_Dome_Height+WC_MB_Tank_Airgap/2;
	G4cout << "delta= " << delta/m << "m \n";
	G4Translate3D translate_dome(0,0,delta*.999999);//for some reason the dome disappears when I use delta alone. Not sure what is going on here.
	G4UnionSolid* WC_MB_Tank_Airgap_volume_dome=new G4UnionSolid("Airgap and dome",WC_MB_Tank_Airgap_volume,WC_MB_Cavern_ceiling,translate_dome*rotation_dome);//hopefully rotates first, then translates
	//**********************
	
	
	// Now put the cavern together with a union.
	G4ThreeVector ztrans= G4ThreeVector(0.0,0.0,(WC_MB_Tank_depth+WC_MB_Tank_Airgap)/2);  // position measured relative to center of volumes.
	G4UnionSolid* WC_Cavern=new G4UnionSolid("Cavern",WC_MB_tank_H20,WC_MB_Tank_Airgap_volume_dome,0,ztrans);
	
	//Now declare the cavern logical volume with air (I like to breathe).
	G4LogicalVolume* logic_WC_Cavern=new G4LogicalVolume(WC_Cavern,G4Material::GetMaterial("Air"),"Cavern",0,0,0);
	
	G4VisAttributes* showColor = new G4VisAttributes(G4Colour(0.0,1.0,0.0)); // Note that I should make this visible, at least initially.
	if (debugMode)		
		logic_WC_Cavern->SetVisAttributes(showColor);
	else	 
		logic_WC_Cavern->SetVisAttributes(G4VisAttributes::Invisible); //amb79
	
	//Make Logical Water tank. This is just the same volume as  WC_MB_tank_H20, but now filled with water
	G4LogicalVolume* logic_WC_MB_tank_H20= new G4LogicalVolume(WC_MB_tank_H20,G4Material::GetMaterial(water),"Tank",0,0,0);
	
	
	//Make Fiducial Volume.
	G4Box* WC_MB_Fiducial = new G4Box ("Fiducial Box",WC_MB_Fid_Length/2.,WC_MB_Fid_Width/2.,WC_MB_Fid_Depth/2.);
	G4LogicalVolume* logic_WC_MB_Fiducial= new G4LogicalVolume(WC_MB_Fiducial,G4Material::GetMaterial(water),"Fiducial_Log",0,0,0);
	
	//these volumes will be placed into the cavern just before we exit.
	
	
	
	//Now make 6 active Layers which will contain the PMT's and Black plastic as daughter volumes. These volumes are sized to the surface area
	//of six faces of the fiducial volume.
	
	//WC_Active_LxW_minus---nomenclature is that this is the face of dimensions LxW located on the negative Z axis (PMT looking upward)
	//WC_Active_LxW_plus---nomenclature is that this is the face of dimensions LxW located on the positive Z axis (PMTlooking downward)
	//WC_Active_LxD_minus---nomenclature is that this is the face of dimensions LxD located on the negative Y axis (PMTlooking toward +Y)
	//WC_Active_LxD_plus---nomenclature is that this is the face of dimensions LxD located on the positive Y axis (PMTlooking toward -Y)
	//WC_Active_WxD_minus---nomenclature is that this is the face of dimensions WxD located on the negative X axis (PMTlooking toward +X)
	//WC_Active_WxD_plus---nomenclature is that this is the face of dimensions WxD located on the positive X axis (PMTlooking toward -X)
	
	
	// need PMT's first. Just a copy paste from Cylindrical detector
	
	//////////// M Fechner : I need to  declare the PMT  here in order to
	// place the PMTs in the truncated cells
	//-----------------------------------------------------
	// The PMT
	//-----------------------------------------------------
	
	////////////J Felde: The PMT logical volume is now constructed separately 
    //from any specific detector geometry so that both the Cylindrical and 
    //Mailbox geometries
	//  can use the same definition. 
	//K.Zbiri: The PMT volume and the PMT glass are now put in parallel. 
    //The PMT glass is the sensitive volume in this new configuration.
	
	ConstructPMT();
	//That's all!	
	
	//Now calculate the L,W,D offsets and # pmt's from the requested spacing.
	//	G4double WCPMT_surfacearea=2*pi*WCPMTRadius*WCPMTRadius;//this is the surface area of a hemisphere
//	G4double WCPMT_crossarea=pi*WCPMTRadius*WCPMTRadius;// this is just the front face crossectional area---this is our standard definition
	G4double WCPMT_crossarea=pi*fPMTConfigs[0].GetRadius()*fPMTConfigs[0].GetRadius();// this is just the front face crossectional area---this is our standard definition
	G4double WC_MB_PMT_Spacing = sqrt(100.*WCPMT_crossarea/WCPMTPercentCoverage);	// factor 100 to conver % to fraction

	//**************************try to use replica--here I am making the unit PMT Cell  ***replica
	//make fundamental square PMT cell, equal to PMT spacing on large side and depth=WC_ActiveLayer_Depth--I will put PMT and blacksheet in this volume  ***replica
	G4Box* WC_PMT_Cell = new G4Box("PMT_cell",WC_MB_PMT_Spacing/2,WC_MB_PMT_Spacing/2,WC_ActiveLayer_Depth/2);	//***replica
	G4LogicalVolume* logic_WC_PMT_Cell= new G4LogicalVolume(WC_PMT_Cell,G4Material::GetMaterial(water),"logic_PMT_Cell",0,0,0);	//***replica
	G4Box* WC_Blacksheet_Cell = new G4Box("WC_BlackSheet_Cell",WC_MB_PMT_Spacing/2,WC_MB_PMT_Spacing/2,WCBlackSheetThickness/2.);		//blacksheet material to fit in cell ***replica
	G4LogicalVolume* logic_WC_Blacksheet_Cell=new G4LogicalVolume(WC_Blacksheet_Cell,G4Material::GetMaterial("Blacksheet"),"logic_WC_Blacksheet_Cell",0,0,0);	//***replica
	// Lets put this stuff+PMT into the cell		//***replica
	//First the glass					//***replica
	G4VPhysicalVolume* phys_WC_GlassFacePMT_Cell=	//this id is needed in order to enable some surface features	//***replica
	new G4PVPlacement(0,                         // no rotation			//***replica
					  G4ThreeVector(0., 0., -WC_ActiveLayer_Depth/2.+WCBlackSheetThickness-PMTOffset),                   // its position	//***replica//"-PMTOffset" butts glass envelope up to blacksheet-aah 10/11/2010
					  logicGlassFaceWCPMT,                // its logical volume   //***replica
					  "WCPMTGlass",// its name //***replica
					  logic_WC_PMT_Cell,         // its mother volume	//***replica
					  false,                     // no boolean os	//***replica
					  0);   
	// Next the PMT (sensitive volume)	//***replica
	G4VPhysicalVolume* phys_WC_PMT_Cell=	//***replica
	new G4PVPlacement(0,                         // no rotation	//***replica
					  G4ThreeVector(0., 0., -WC_ActiveLayer_Depth/2.+WCBlackSheetThickness-PMTOffset),                   // its position	//***replica//"-PMTOffset" butts glass envelope up to blacksheet-aah 10/11/2010
					  logicWCPMT,                // its logical volume	//***replica
					  "WCPMT",// its name		//***replica
					  logic_WC_PMT_Cell,         // its mother volume	//***replica
					  false,                     // no boolean os		//***replica
					  0);	//***replica
	// Next the blacksheet	//***replica
	G4VPhysicalVolume* phys_WC_Blacksheet_Cell=	//***replica
	new G4PVPlacement(0,                         // no rotation	//***replica
					  G4ThreeVector(0,0,(-WC_ActiveLayer_Depth+WCBlackSheetThickness)/2),    // its position	//***replica
					  logic_WC_Blacksheet_Cell,                // its logical volume	//***replica
					  "LW_Blacksheet", // its name	//***replica
					  logic_WC_PMT_Cell,         // its mother volume	//***replica
					  false,                     // no boolean os	//***replica
					  0);                        // copy #.				  //***replica
	
	// Turn on the special features of the surface	//***replica
	//glass reflections				//***replica
	new G4LogicalBorderSurface("GlassCathodeSurface",	//***replica
							   phys_WC_GlassFacePMT_Cell,phys_WC_PMT_Cell,	//***replica
							   OpGlassCathodeSurface); //***replica
	// Blacksheet reflections	//***replica
	new G4LogicalBorderSurface("WaterBS_LW_PolySurface",	//***replica
							   phys_WC_PMT_Cell,phys_WC_Blacksheet_Cell,	//***replica
							   OpWaterBSSurface);	//***replica
	
	if (!debugMode)	{
		logic_WC_PMT_Cell->SetVisAttributes(G4VisAttributes::Invisible);//*replica
		logic_WC_Blacksheet_Cell->SetVisAttributes(G4VisAttributes::Invisible);//*replica
	}
	//*******************************************************************************	//***replica
	



	//Length dimension
	G4int WC_MB_NumPMT_L		=(1+ int((WC_MB_Fid_Length-2*sphereRadius)/WC_MB_PMT_Spacing));	//# PMT's along length
	G4double WC_MB_PMT_offset_L	=(WC_MB_Fid_Length-(WC_MB_NumPMT_L-1)*WC_MB_PMT_Spacing)/2;		//Offset in length dimension
	//Width
	G4int WC_MB_NumPMT_W		=(1+ int((WC_MB_Fid_Width-2*sphereRadius)/WC_MB_PMT_Spacing));	//# PMT's along width
	G4double WC_MB_PMT_offset_W	=(WC_MB_Fid_Width-(WC_MB_NumPMT_W-1)*WC_MB_PMT_Spacing)/2;	//Offset in width dimension
	//Depth
	G4int WC_MB_NumPMT_D		=(1+ int((WC_MB_Fid_Depth-2*sphereRadius)/WC_MB_PMT_Spacing));	//# PMT's along depth
	G4double WC_MB_PMT_offset_D	=(WC_MB_Fid_Depth-(WC_MB_NumPMT_D-1)*WC_MB_PMT_Spacing)/2;	//Offset in depth dimension
	G4cout << "PMT Length Offset (m) "<< WC_MB_PMT_offset_L/m << "PMT Width Offset (m) " << WC_MB_PMT_offset_W/m<<"PMT Depth Offset (m) "<<WC_MB_PMT_offset_D/m<<"\n";
	
	//WC_Active_LxW---perp to Z axis
	
	G4Box* WC_Active_LxW = new G4Box("LxW",WC_MB_Fid_Length/2+WC_ActiveLayer_Depth,WC_MB_Fid_Width/2+WC_ActiveLayer_Depth,WC_ActiveLayer_Depth/2);	//active volume with PMT's 
	G4LogicalVolume* logic_WC_Active_LxW_minus=new G4LogicalVolume(WC_Active_LxW,G4Material::GetMaterial(water),"LxWActive_minus",0,0,0);
	G4LogicalVolume* logic_WC_Active_LxW_plus=new G4LogicalVolume(WC_Active_LxW,G4Material::GetMaterial(water),"LxWActive_plus",0,0,0);
	G4VPhysicalVolume* phys_WC_Active_LxW_minus;
	G4VPhysicalVolume* phys_WC_ActiveGlass_LxW_minus;
	G4VPhysicalVolume* phys_WC_Active_LxW_plus;
	G4VPhysicalVolume* phys_WC_ActiveGlass_LxW_plus;
	G4Box* WC_BlacksheetLxW = new G4Box("WC_BlackSheetLxW",WC_MB_Fid_Length/2+WC_ActiveLayer_Depth-WCBlackSheetThickness,
										WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness,WCBlackSheetThickness/2.);		//blacksheet material	
	//*************************replica***********************
	
	//need to cutout center rectangle that will fit the replica pmtvolumes (already with blacksheet)   ***replica
	G4Box* WC_BlacksheetLxW_donuthole = new G4Box("WC_BlackSheetLxW_donuthole",WC_MB_NumPMT_L*WC_MB_PMT_Spacing/2, //***replica
												  WC_MB_NumPMT_W*WC_MB_PMT_Spacing/2,WCBlackSheetThickness/2.);				//***replica
	
	G4SubtractionSolid*  WC_BlacksheetLxW_donut= new G4SubtractionSolid("WC_BlacksheetLxW_donut",WC_BlacksheetLxW,WC_BlacksheetLxW_donuthole,0,G4ThreeVector(0.,0.,0.));	//this is now the blacksheet that will suround the PMT volumes  ***replica							
	//***replica
	
	G4LogicalVolume* logic_WC_BlacksheetLxW = new G4LogicalVolume(WC_BlacksheetLxW_donut,G4Material::GetMaterial("Blacksheet"),"LagicBlacksheet_LxW",0,0,0);				 //***replica														
	
	//*********************************replica*******************************



	G4Box* WC_BlacksheetLxW_corner_L= new G4Box("WC_BlackSheetLxW_corner_L",WC_MB_Fid_Length/2+WC_ActiveLayer_Depth,WCBlackSheetThickness/2.,WC_ActiveLayer_Depth/2.);//Blacksheeet corner--to lite-tite the fiducial volume--This should already be oriented correctly
	G4LogicalVolume* logic_WC_BlacksheetLxW_corner_L=new G4LogicalVolume(WC_BlacksheetLxW_corner_L,G4Material::GetMaterial("Blacksheet"),"LagicBlacksheet_LxW",0,0,0);
	G4Box* WC_BlacksheetLxW_corner_W= new G4Box("WC_BlackSheetLxW_corner_W",WCBlackSheetThickness/2.,WC_MB_Fid_Width/2+WC_ActiveLayer_Depth,WC_ActiveLayer_Depth/2.);//Blacksheet corner--lite-tite the fiducial volume--This should already be oriented correctly
	G4LogicalVolume* logic_WC_BlacksheetLxW_corner_W=new G4LogicalVolume(WC_BlacksheetLxW_corner_W,G4Material::GetMaterial("Blacksheet"),"LagicBlacksheet_LxW",0,0,0);
	
	
	G4double xmin=WC_MB_Fid_Length, xmax=-WC_MB_Fid_Length, ymin=WC_MB_Fid_Width, ymax=-WC_MB_Fid_Width;
	G4int icopy=0,num_pmt=0;

	//***replica
	// will build up pmt volume in two steps, one making a row of pmt volumes, then making a rectangle out of the rows.
	
	G4Box *LxW_PMT_Row =	new G4Box("LxW_PMT_Row",WC_MB_NumPMT_L*WC_MB_PMT_Spacing/2,WC_MB_PMT_Spacing/2,WC_ActiveLayer_Depth/2.);
	G4LogicalVolume* logic_LxW_PMT_Row= new G4LogicalVolume(LxW_PMT_Row,G4Material::GetMaterial(water),"logic_LxW_PMT_Row",0,0,0);
	G4Box * LxW_PMT_Volume=new G4Box("LxW_PMT_Volume",WC_MB_NumPMT_L*WC_MB_PMT_Spacing/2,WC_MB_NumPMT_W*WC_MB_PMT_Spacing/2,WC_ActiveLayer_Depth/2);	//this is entire PMT volume
	G4LogicalVolume* logic_LxW_PMT_Volume= new G4LogicalVolume(LxW_PMT_Volume,G4Material::GetMaterial(water),"logic_LxW_PMT_Volume",0,0,0);
	
	G4VPhysicalVolume* physiWC_LxW_PMT_Row = 
	new G4PVReplica("WC_PMT_Row",
					logic_WC_PMT_Cell,
					logic_LxW_PMT_Row,
					kXAxis,
					WC_MB_NumPMT_L,
					WC_MB_PMT_Spacing,
					0.); 
	
	G4VPhysicalVolume* physiWC_LxW_PMT_Volume = 
	new G4PVReplica("WC_LxW_PMT_Volume",
					logic_LxW_PMT_Row,
					logic_LxW_PMT_Volume,
					kYAxis,
					WC_MB_NumPMT_W,
					WC_MB_PMT_Spacing,
					0.); 
	
	//Now we should have PMT's placed in the PMT "Donut_Hole"
	if (!debugMode)	{
		logic_LxW_PMT_Row->SetVisAttributes(G4VisAttributes::Invisible);//*replica
		logic_LxW_PMT_Volume->SetVisAttributes(G4VisAttributes::Invisible);//*replica
	}
	//Now put into face volumes
	phys_WC_Active_LxW_minus =					// put PMT's into bottom face--eventually it will be translated to bottom face
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0.,0.,0.),                   // its position
					  logic_LxW_PMT_Volume,                // its logical volume
					  //"LxW_PMT_minus", // its name
					  "WCPMT",// (JF) replaced above  
					  logic_WC_Active_LxW_minus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // every PMT needs an unique id in this physical volume		??? what to do here!??
	
	
	phys_WC_Active_LxW_plus =					// put PMT's into bottom face--eventually it will be translated to bottom face
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0.,0.,0.),                   // its position
					  logic_LxW_PMT_Volume,                // its logical volume
					  //"LxW_PMT_minus", // its name
					  "WCPMT",// (JF) replaced above  
					  logic_WC_Active_LxW_plus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // every PMT needs an unique id in this physical volume		??? what to do here!??
	
	//end ****replica					

	G4cout << "total on LxW Surface: " << 2*WC_MB_NumPMT_L*WC_MB_NumPMT_W << "\n";	//***replica (modded original)
	G4cout << "Coverage was calculated to be: " << (WC_MB_NumPMT_L*WC_MB_NumPMT_W *WCPMT_crossarea/(WC_MB_Fid_Length*WC_MB_Fid_Width)) << "\n";
	num_pmt+=2*WC_MB_NumPMT_L*WC_MB_NumPMT_W;  //***replica	

	
	//add blacksheet
	
	G4ThreeVector position_blacksheet_LxW =G4ThreeVector(0,0,(-WC_ActiveLayer_Depth+WCBlackSheetThickness)/2) ;//hopefully this puts blacksheet on bottom of volume.
	
	G4VPhysicalVolume*	phys_WC_Active_LxW_blacksheet_minus =
	new G4PVPlacement(0,                         // no rotation
					  position_blacksheet_LxW,    // its position
					  logic_WC_BlacksheetLxW,                // its logical volume
					  "LW_Blacksheet", // its name 
					  logic_WC_Active_LxW_minus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // copy #.
	
	phys_WC_Active_LxW_blacksheet_minus =					//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0,-(WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0),    // its position
					  logic_WC_BlacksheetLxW_corner_L,                // its logical volume
					  "LW_Blacksheet_corner_L", // its name 
					  logic_WC_Active_LxW_minus,         // its mother volume
					  false,                     // no boolean os
					  1);	// copy #.
	phys_WC_Active_LxW_blacksheet_minus =					//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0,(WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0),    // its position opposite side
					  logic_WC_BlacksheetLxW_corner_L,                // its logical volume
					  "LW_Blacksheet_corner_L", // its name 
					  logic_WC_Active_LxW_minus,         // its mother volume
					  false,                     // no boolean os
					  2); // copy #.
	
	phys_WC_Active_LxW_blacksheet_minus =					//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(-(WC_MB_Fid_Length/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0,0),    // its position opposite side
					  logic_WC_BlacksheetLxW_corner_W,                // its logical volume
					  "LW_Blacksheet_corner_W", // its name 
					  logic_WC_Active_LxW_minus,         // its mother volume
					  false,                     // no boolean os
					  3); // copy #.
	
	
	phys_WC_Active_LxW_blacksheet_minus =					//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector((WC_MB_Fid_Length/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0,0),    // its position opposite side
					  logic_WC_BlacksheetLxW_corner_W,                // its logical volume
					  "LW_Blacksheet_corner_W", // its name 
					  logic_WC_Active_LxW_minus,         // its mother volume
					  false,                     // no boolean os
					  4); // copy #.
	
	//	Now do the same for the positive side pmt layer	
	
	G4VPhysicalVolume*	phys_WC_Active_LxW_blacksheet_plus =
	new G4PVPlacement(0,                         // no rotation
					  position_blacksheet_LxW,    // its position
					  logic_WC_BlacksheetLxW,                // its logical volume
					  "LW_Blacksheet", // its name 
					  logic_WC_Active_LxW_plus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // copy #.
	
	
	phys_WC_Active_LxW_blacksheet_plus =	//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0,-(WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0),    // its position opposite side
					  logic_WC_BlacksheetLxW_corner_L,                // its logical volume
					  "LW_Blacksheet_corner_L", // its name 
					  logic_WC_Active_LxW_plus,         // its mother volume
					  false,                     // no boolean os
					  1); 										
	
	phys_WC_Active_LxW_blacksheet_plus =	//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0,(WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0),    // its position opposite side
					  logic_WC_BlacksheetLxW_corner_L,                // its logical volume
					  "LW_Blacksheet_corner_L", // its name 
					  logic_WC_Active_LxW_plus,         // its mother volume
					  false,                     // no boolean os
					  2); 
	
	phys_WC_Active_LxW_blacksheet_plus =	//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(-(WC_MB_Fid_Length/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0,0),    // its position opposite side
					  logic_WC_BlacksheetLxW_corner_W,                // its logical volume
					  "LW_Blacksheet_corner_W", // its name 
					  logic_WC_Active_LxW_plus,         // its mother volume
					  false,                     // no boolean os
					  3); 
	
	
	phys_WC_Active_LxW_blacksheet_plus =	//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector((WC_MB_Fid_Length/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0,0),    // its position opposite side
					  logic_WC_BlacksheetLxW_corner_W,                // its logical volume
					  "LW_Blacksheet_corner_W", // its name 
					  logic_WC_Active_LxW_plus,         // its mother volume
					  false,                     // no boolean os
					  4); 
	
	
	new G4LogicalBorderSurface("WaterBS_LW_PolySurface",
							   phys_WC_Active_LxW_minus,phys_WC_Active_LxW_blacksheet_minus,
							   OpWaterBSSurface);
	new G4LogicalBorderSurface("WaterBS_LW_PolySurface",
							   phys_WC_Active_LxW_plus,phys_WC_Active_LxW_blacksheet_plus,
							   OpWaterBSSurface);																						
	
	
	//visualization if debugMode=true																						
	G4VisAttributes* WC_BlacksheetLxWVisAtt = new G4VisAttributes(G4Colour(0,0.0,1.));
	G4VisAttributes* WC_BlacksheetLxWVisAtt_corner = new G4VisAttributes(G4Colour(1.,0.0,1.));
	if(debugMode){
		logic_WC_BlacksheetLxW->SetVisAttributes(WC_BlacksheetLxWVisAtt);
		logic_WC_BlacksheetLxW_corner_L->SetVisAttributes(WC_BlacksheetLxWVisAtt);
		logic_WC_BlacksheetLxW_corner_W->SetVisAttributes(WC_BlacksheetLxWVisAtt_corner);
	}
	else {
		logic_WC_BlacksheetLxW->SetVisAttributes(G4VisAttributes::Invisible);
		logic_WC_BlacksheetLxW_corner_L->SetVisAttributes(G4VisAttributes::Invisible);
		logic_WC_BlacksheetLxW_corner_W->SetVisAttributes(G4VisAttributes::Invisible);
	}
	
	
	
	//At this stage we should have the bottom layer containing the PMT's and blacksheet defined--but located in the middle of the water tank
	//Now I will make the translation/rotation which will place the top and bottom layers just outsidethe fiducial detector (in the logical watertank volume.
	
	G4Translate3D LWTranslation_plus(0., 0., +(WC_MB_Fid_Depth+WC_ActiveLayer_Depth)/2);				//translation to top of Fiducial surface (+z)
	G4Translate3D LWTranslation_minus(0., 0., -(WC_MB_Fid_Depth+WC_ActiveLayer_Depth)/2);				//Translation to bottom surface
	const G4Transform3D rotation_LxW_plus = G4RotateX3D(pi);												//rotate to looking in -z direction (180 degrees) for top face
	
	//ok, put up the sides!
	// First side is on the negative axis--this just requires a translation
	new G4PVPlacement(LWTranslation_minus  ,           // no rotation , translation only
					  logic_WC_Active_LxW_minus,     // its logical volume				  
					  "LW_minus",        // its name
					  logic_WC_MB_tank_H20,      // its mother  volume, the WC water tank
					  false,           // no boolean operations
					  0);              // copy number 
	
	new G4PVPlacement(LWTranslation_plus*rotation_LxW_plus, // rotate +180 degrees, then translate upward.
					  logic_WC_Active_LxW_plus,     // its logical volume				  
					  "LW_plus",        // its name
					  logic_WC_MB_tank_H20,      // its mother  volume, the WC water tank
					  false,           // no boolean operations
					  1);              // copy number 
	
	
	
	
	
	//Next two faces
	//WC_Active_WxD	(located on -x axis) I will build this as if it were in the x,y plane (like the LW above) , then rotate it +90degrees around the y axis to make the -x side				 
	
	
	G4Box* WC_Active_WxD=new G4Box("WxD",WC_MB_Fid_Depth/2,WC_MB_Fid_Width/2+WC_ActiveLayer_Depth,WC_ActiveLayer_Depth/2);						//active volume--increase width to cover ends to seal with blacksheet
	G4LogicalVolume* logic_WC_Active_WxD_minus = new G4LogicalVolume(WC_Active_WxD,G4Material::GetMaterial(water),"WxDActive_minus",0,0,0);
	G4LogicalVolume* logic_WC_Active_WxD_plus = new G4LogicalVolume(WC_Active_WxD,G4Material::GetMaterial(water),"WxDActive_plus",0,0,0);
	G4Box* WC_BlacksheetWxD = new G4Box("WC_BlackSheetWxD",WC_MB_Fid_Depth/2,WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2.,WCBlackSheetThickness/2.);			//blacksheet material	

	//*********************************************replica*******************************
	//need to cutout center rectangle that will fit the replica pmtvolumes (already with blacksheet)   ***replica
	G4Box* WC_BlacksheetWxD_donuthole = new G4Box("WC_BlackSheetWxD_donuthole",WC_MB_NumPMT_D*WC_MB_PMT_Spacing/2, //***replica
												  WC_MB_NumPMT_W*WC_MB_PMT_Spacing/2,WCBlackSheetThickness/2.);				//***replica
	
	G4SubtractionSolid*  WC_BlacksheetWxD_donut= new G4SubtractionSolid("WC_BlacksheetWxD_donut",WC_BlacksheetWxD,WC_BlacksheetWxD_donuthole,0,G4ThreeVector(0.,0.,0.));	//this is now the blacksheet that will suround the PMT volumes  ***replica							
	//***replica
	
	G4LogicalVolume* logic_WC_BlacksheetWxD = new G4LogicalVolume(WC_BlacksheetWxD_donut,G4Material::GetMaterial("Blacksheet"),"LagicBlacksheet_WxD",0,0,0);				 //***replica														
	
	
	
	//*********************************************replica*******************************
	

//	G4LogicalVolume* logic_WC_BlacksheetWxD = new G4LogicalVolume(WC_BlacksheetWxD,G4Material::GetMaterial("Blacksheet"),"LagicBlacksheet_WxD",0,0,0);							  
	G4Box* WC_BlacksheetWxD_corner= new G4Box("WC_BlackSheetWxD_corner",WC_MB_Fid_Depth/2,WCBlackSheetThickness/2.,WC_ActiveLayer_Depth/2.);//Blacksheet corner---This should already be oriented correctly
	G4LogicalVolume* logic_WC_BlacksheetWxD_corner=new G4LogicalVolume(WC_BlacksheetWxD_corner,G4Material::GetMaterial("Blacksheet"),"LagicBlacksheet_WxD_corner",0,0,0);
	
	
	
	G4VPhysicalVolume* phys_WC_Active_WxD_plus, * phys_WC_ActiveGlass_WxD_plus;
	G4VPhysicalVolume* phys_WC_Active_WxD_minus, * phys_WC_ActiveGlass_WxD_minus;								  
	
	
	icopy=0;
	xmin=WC_MB_Fid_Depth, xmax=-WC_MB_Fid_Depth, ymin=WC_MB_Fid_Width, ymax=-WC_MB_Fid_Width;
	//***replica
	// will build up pmt volume in two steps, one making a row of pmt volumes, then making a rectangle out of the rows. for this config, "D" runs on X axis, "W" is on Y axis
	
	
	G4Box * WxD_PMT_Row =	new G4Box("WxD_PMT_Row",WC_MB_NumPMT_D*WC_MB_PMT_Spacing/2,WC_MB_PMT_Spacing/2,WC_ActiveLayer_Depth/2.);
	G4LogicalVolume* logic_WxD_PMT_Row= new G4LogicalVolume(WxD_PMT_Row,G4Material::GetMaterial(water),"logic_WxD_PMT_Row",0,0,0);
	G4Box * WxD_PMT_Volume=new G4Box("WxD_PMT_Volume",WC_MB_NumPMT_D*WC_MB_PMT_Spacing/2,WC_MB_NumPMT_W*WC_MB_PMT_Spacing/2,WC_ActiveLayer_Depth/2);	//this is entire PMT volume
	G4LogicalVolume* logic_WxD_PMT_Volume= new G4LogicalVolume(WxD_PMT_Volume,G4Material::GetMaterial(water),"logic_WxD_PMT_Volume",0,0,0);
	
	G4VPhysicalVolume* physiWC_WxD_PMT_Row = 
	new G4PVReplica("WC_PMT_Row",
					logic_WC_PMT_Cell,
					logic_WxD_PMT_Row,
					kXAxis,
					WC_MB_NumPMT_D,
					WC_MB_PMT_Spacing,
					0.); 
	
	G4VPhysicalVolume* physiWC_WxD_PMT_Volume = 
	new G4PVReplica("WC_PMT_Volume",
					logic_WxD_PMT_Row,
					logic_WxD_PMT_Volume,
					kYAxis,
					WC_MB_NumPMT_W,
					WC_MB_PMT_Spacing,
					0.); 
	
	//Now we should have PMT's placed in the PMT "Donut_Hole"
	
	//Now put into face volumes
	phys_WC_Active_WxD_minus =					// put PMT's into bottom face--eventually it will be translated to bottom face
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0.,0.,0.),                   // its position
					  logic_WxD_PMT_Volume,                // its logical volume
					  //"LxW_PMT_minus", // its name
					  "WCPMT",// (JF) replaced above  
					  logic_WC_Active_WxD_minus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // every PMT needs an unique id in this physical volume		??? what to do here!??
	
	
	phys_WC_Active_WxD_plus =					// put PMT's into bottom face--eventually it will be translated to bottom face
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0.,0.,0.),                   // its position
					  logic_WxD_PMT_Volume,                // its logical volume
					  //"LxW_PMT_minus", // its name
					  "WCPMT",// (JF) replaced above  
					  logic_WC_Active_WxD_plus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // every PMT needs an unique id in this physical volume		??? what to do here!??
	
	
	
	if (!debugMode)	{
		logic_WxD_PMT_Row->SetVisAttributes(G4VisAttributes::Invisible);//*replica
		logic_WxD_PMT_Volume->SetVisAttributes(G4VisAttributes::Invisible);//*replica
	}
	
	
	//end ****replica	
	
								
	G4cout << "total on WxD Surface: " << 2*WC_MB_NumPMT_W*WC_MB_NumPMT_D << "\n";//****replica modified original
	G4cout << "Coverage was calculated to be: " << (WC_MB_NumPMT_W*WC_MB_NumPMT_D*WCPMT_crossarea/(WC_MB_Fid_Width*WC_MB_Fid_Depth)) << "\n";// ***replica--modified original
	num_pmt+=2*WC_MB_NumPMT_W*WC_MB_NumPMT_D;  //***replica	
	
	//				G4cout << "halflength, xmin, xmax= " << WC_MB_Fid_Depth/2<<" "<<xmin<<" "<<xmax << "\n";
	//				G4cout << "halfwidth, ymin, ymax=" << WC_MB_Fid_Width/2<<" "<<ymin<<" "<<ymax << "\n";
	//				G4cout << "\n";							
	
	//add blacksheet----on the corners, only need to put it on the +-Width/2 sides since the cap covers the LxW parts	
	
	G4ThreeVector position_blacksheet_WxD =G4ThreeVector(0,0,(-WC_ActiveLayer_Depth+WCBlackSheetThickness)/2) ;//hopefully this puts blacksheet on bottom of volume.
	
	G4VPhysicalVolume*	phys_WC_Active_WxD_blacksheet_minus =
	new G4PVPlacement(0,                         // no rotation
					  position_blacksheet_WxD,    // its position
					  logic_WC_BlacksheetWxD,                // its logical volume
					  "WD_Blacksheet", // its name 
					  logic_WC_Active_WxD_minus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // copy #.
	
	
	
	phys_WC_Active_WxD_blacksheet_minus =//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0,-(WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0),    // its position opposite side
					  logic_WC_BlacksheetWxD_corner,                // its logical volume
					  "LagicBlacksheet_WxD_corner", // its name 
					  logic_WC_Active_WxD_minus,         // its mother volume
					  false,                     // no boolean os
					  1); 
	
	
	phys_WC_Active_WxD_blacksheet_minus =//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0,(WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0),    // its position opposite side
					  logic_WC_BlacksheetWxD_corner,                // its logical volume
					  "LagicBlacksheet_WxD_corner", // its name 
					  logic_WC_Active_WxD_minus,         // its mother volume
					  false,                     // no boolean os
					  2); 																										
	
	
	
	
	
	
	
	G4VPhysicalVolume*	phys_WC_Active_WxD_blacksheet_plus =
	new G4PVPlacement(0,                         // no rotation
					  position_blacksheet_WxD,    // its position
					  logic_WC_BlacksheetWxD,                // its logical volume
					  "WD_Blacksheet", // its name 
					  logic_WC_Active_WxD_plus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // copy #.
	
	
	
	
	phys_WC_Active_WxD_blacksheet_plus =//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0,-(WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0),    // its position opposite side
					  logic_WC_BlacksheetWxD_corner,                // its logical volume
					  "WLagicBlacksheet_WxD_corner", // its name 
					  logic_WC_Active_WxD_plus,         // its mother volume
					  false,                     // no boolean os
					  1); 
	
	
	phys_WC_Active_WxD_blacksheet_plus =//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0,(WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0),    // its position opposite side
					  logic_WC_BlacksheetWxD_corner,                // its logical volume
					  "LagicBlacksheet_WxD_corner", // its name 
					  logic_WC_Active_WxD_plus,         // its mother volume
					  false,                     // no boolean os
					  2); 																										
	
	
	new G4LogicalBorderSurface("WaterBS_LW_PolySurface",
							   phys_WC_Active_WxD_minus,phys_WC_Active_WxD_blacksheet_minus,
							   OpWaterBSSurface);
	new G4LogicalBorderSurface("WaterBS_LW_PolySurface",
							   phys_WC_Active_WxD_plus,phys_WC_Active_WxD_blacksheet_plus,
							   OpWaterBSSurface);																						
	
	
	G4VisAttributes* WC_BlacksheetWxDVisAtt = new G4VisAttributes(G4Colour(0.0,0.,1.));
	G4VisAttributes* WC_BlacksheetWxDVisAtt_corner = new G4VisAttributes(G4Colour(1.,0.0,1.));
	if(debugMode){
		logic_WC_BlacksheetWxD->SetVisAttributes(WC_BlacksheetWxDVisAtt);
		logic_WC_BlacksheetWxD_corner->SetVisAttributes(WC_BlacksheetWxDVisAtt_corner);
	}
	else {
		logic_WC_BlacksheetWxD->SetVisAttributes(G4VisAttributes::Invisible);
		logic_WC_BlacksheetWxD_corner->SetVisAttributes(G4VisAttributes::Invisible);
	}
	
	
	//At this stage we should have the bottom layer containing the PMT's and blacksheet defined
	
	//Now I will rotate this volume by +90degrees on Y axis
	const G4Transform3D rotation_WxD_minus = G4RotateY3D(pi/2.);				// minus rotates +90degrees around Y axis
	const G4Transform3D rotation_WxD_plus = G4RotateY3D(-pi/2.);				// plus rotates -90degrees around Y axis
	G4Translate3D WDTranslate_plus( (WC_MB_Fid_Length+WC_ActiveLayer_Depth)/2., 0., 0);		//for +x axis
	G4Translate3D WDTranslate_minus( -(WC_MB_Fid_Length+WC_ActiveLayer_Depth)/2.,0.,0.);	// for -x axis
	G4cout << "-x axis offset for translation" << -((WC_MB_Fid_Length+WC_ActiveLayer_Depth)/2.)/m << "m"<<"\n";
	
	
	//ok, put up the sides!
	// First side is on the negative axis--this requires rotation and translation for this face 
	
	new G4PVPlacement(WDTranslate_minus*rotation_WxD_minus,					// rotate from xy plane to xz plane 
					  logic_WC_Active_WxD_minus,				// its logical volume				  
					  "WD_minus",					// its name
					  logic_WC_MB_tank_H20,			// its mother  volume, the WC water tank
					  false,						// no boolean operations
					  2);							// copy number
	// Ok now the positive face.
	new G4PVPlacement(WDTranslate_plus*rotation_WxD_plus,					// rotate from xy plane to xz plane 
					  logic_WC_Active_WxD_plus,				// its logical volume				  
					  "WD_plus",					// its name
					  logic_WC_MB_tank_H20,			// its mother  volume, the WC water tank
					  false,						// no boolean operations
					  3);							// copy number 
	
	
	
	//Last two faces
	
	//WC_Active_LxD	(located on -y axis) I will build this as if it were in the x,y plane (like the LW above) , then rotate it +90degrees around the x axis to make the -y side				 
	
	
	G4Box* WC_Active_LxD = new G4Box("LxD",WC_MB_Fid_Length/2,WC_MB_Fid_Depth/2,WC_ActiveLayer_Depth/2);//active volume
	G4LogicalVolume* logic_WC_Active_LxD_minus = new G4LogicalVolume(WC_Active_LxD,G4Material::GetMaterial(water),"LxDActive_minus",0,0,0);
	G4LogicalVolume* logic_WC_Active_LxD_plus = new G4LogicalVolume(WC_Active_LxD,G4Material::GetMaterial(water),"LxDActive_plus",0,0,0);
	G4Box* WC_BlacksheetLxD = new G4Box("WC_BlackSheetLxD",WC_MB_Fid_Length/2,WC_MB_Fid_Depth/2,WCBlackSheetThickness/2.);//blacksheet material	

	//*********************************************replica*******************************
	//need to cutout center rectangle that will fit the replica pmtvolumes (already with blacksheet)   ***replica
	G4Box* WC_BlacksheetLxD_donuthole = new G4Box("WC_BlackSheetLxD_donuthole",WC_MB_NumPMT_L*WC_MB_PMT_Spacing/2, //***replica
												  WC_MB_NumPMT_D*WC_MB_PMT_Spacing/2,WCBlackSheetThickness/2.);				//***replica
	
	G4SubtractionSolid*  WC_BlacksheetLxD_donut= new G4SubtractionSolid("WC_BlacksheetLxD_donut",WC_BlacksheetLxD,WC_BlacksheetLxD_donuthole,0,G4ThreeVector(0.,0.,0.));	//this is now the blacksheet that will suround the PMT volumes  ***replica							
	//***replica
	
	G4LogicalVolume* logic_WC_BlacksheetLxD = new G4LogicalVolume(WC_BlacksheetLxD_donut,G4Material::GetMaterial("Blacksheet"),"LagicBlacksheet_LxD",0,0,0);				 //***replica														
	
	
	
	//*********************************************replica*******************************
	

	G4VPhysicalVolume* phys_WC_Active_LxD_plus, * phys_WC_ActiveGlass_LxD_plus;
	G4VPhysicalVolume* phys_WC_Active_LxD_minus, * phys_WC_ActiveGlass_LxD_minus;								  
	
	// Loop over the L,D dimensions to put ion PMT's
	
	icopy=0;
	xmin=WC_MB_Fid_Length, xmax=-WC_MB_Fid_Length, ymin=WC_MB_Fid_Depth, ymax=-WC_MB_Fid_Depth;

	//***replica
	// will build up pmt volume in two steps, one making a row of pmt volumes, then making a rectangle out of the rows. for this config, "D" runs on X axis, "W" is on Y axis
	
	
	G4Box * LxD_PMT_Row =	new G4Box("LxD_PMT_Row",WC_MB_NumPMT_L*WC_MB_PMT_Spacing/2,WC_MB_PMT_Spacing/2,WC_ActiveLayer_Depth/2.);
	G4LogicalVolume* logic_LxD_PMT_Row= new G4LogicalVolume(LxD_PMT_Row,G4Material::GetMaterial(water),"logic_LxD_PMT_Row",0,0,0);
	G4Box * LxD_PMT_Volume=new G4Box("LxD_PMT_Volume",WC_MB_NumPMT_L*WC_MB_PMT_Spacing/2,WC_MB_NumPMT_D*WC_MB_PMT_Spacing/2,WC_ActiveLayer_Depth/2);	//this is entire PMT volume
	G4LogicalVolume* logic_LxD_PMT_Volume= new G4LogicalVolume(LxD_PMT_Volume,G4Material::GetMaterial(water),"logic_LxD_PMT_Volume",0,0,0);
	
	G4VPhysicalVolume* physiWC_LxD_PMT_Row = 
	new G4PVReplica("WC_PMT_Row",
					logic_WC_PMT_Cell,
					logic_LxD_PMT_Row,
					kXAxis,
					WC_MB_NumPMT_L,
					WC_MB_PMT_Spacing,
					0.); 
	
	G4VPhysicalVolume* physiWC_LxD_PMT_Volume = 
	new G4PVReplica("WC_PMT_Volume",
					logic_LxD_PMT_Row,
					logic_LxD_PMT_Volume,
					kYAxis,
					WC_MB_NumPMT_D,
					WC_MB_PMT_Spacing,
					0.); 
	
	//Now we should have PMT's placed in the PMT "Donut_Hole"
	
	//Now put into face volumes
	phys_WC_Active_LxD_minus =					// put PMT's into bottom face--eventually it will be translated to bottom face
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0.,0.,0.),                   // its position
					  logic_LxD_PMT_Volume,                // its logical volume
					  //"LxD_PMT_minus", // its name
					  "WCPMT",// (JF) replaced above  
					  logic_WC_Active_LxD_minus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // every PMT needs an unique id in this physical volume		??? what to do here!??
	
	
	phys_WC_Active_LxD_plus =					// put PMT's into bottom face--eventually it will be translated to bottom face
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0.,0.,0.),                   // its position
					  logic_LxD_PMT_Volume,                // its logical volume
					  //"LxD_PMT_minus", // its name
					  "WCPMT",// (JF) replaced above  
					  logic_WC_Active_LxD_plus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // every PMT needs an unique id in this physical volume		??? what to do here!??
	
	
	
	if (!debugMode)	{
		logic_LxD_PMT_Row->SetVisAttributes(G4VisAttributes::Invisible);//*replica
		logic_LxD_PMT_Volume->SetVisAttributes(G4VisAttributes::Invisible);//*replica
	}
	
	//end ****replica	
	
	
	//				G4cout << "halflength, xmin, xmax= " << WC_MB_Fid_Length/2<<" "<<xmin<<" "<<xmax << "\n";
	//				G4cout << "halfwidth, ymin, ymax=" << WC_MB_Fid_Depth/2<<" "<<ymin<<" "<<ymax << "\n";
	//				G4cout << "\n";
	G4cout << "total on LxD Surface: " << 2* WC_MB_NumPMT_L* WC_MB_NumPMT_D << "\n";
	G4cout << "Coverage was calculated to be: " << (WC_MB_NumPMT_L* WC_MB_NumPMT_D*WCPMT_crossarea/(WC_MB_Fid_Length*WC_MB_Fid_Depth)) << "\n";
	num_pmt+=2* WC_MB_NumPMT_L* WC_MB_NumPMT_D;
	G4cout << "total number of PMT: " << num_pmt << "\n";	
	
	//add blacksheet
	
	G4ThreeVector position_blacksheet_LxD =G4ThreeVector(0,0,(-WC_ActiveLayer_Depth+WCBlackSheetThickness)/2) ;//hopefully this puts blacksheet on bottom of volume.
	
	G4VPhysicalVolume*	phys_WC_Active_LxD_blacksheet_minus =
	new G4PVPlacement(0,                         // no rotation
					  position_blacksheet_LxD,    // its position
					  logic_WC_BlacksheetLxD,                // its logical volume
					  "LD_Blacksheet", // its name 
					  logic_WC_Active_LxD_minus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // copy #.
	
	G4VPhysicalVolume*	phys_WC_Active_LxD_blacksheet_plus =
	new G4PVPlacement(0,                         // no rotation
					  position_blacksheet_LxD,    // its position
					  logic_WC_BlacksheetLxD,                // its logical volume
					  "LD_Blacksheet", // its name 
					  logic_WC_Active_LxD_plus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // copy #.
	
	
	//Activate the water/blacksheet surfaces in both faces
	new G4LogicalBorderSurface("WaterBS_LW_PolySurface",
							   phys_WC_Active_LxD_minus,phys_WC_Active_LxD_blacksheet_minus,
							   OpWaterBSSurface);
	new G4LogicalBorderSurface("WaterBS_LW_PolySurface",
							   phys_WC_Active_LxD_plus,phys_WC_Active_LxD_blacksheet_plus,
							   OpWaterBSSurface);																						
	
	
	G4VisAttributes* WC_BlacksheetLxDVisAtt = new G4VisAttributes(G4Colour(0.0,0.,1.));
	if(debugMode)
		logic_WC_BlacksheetLxD->SetVisAttributes(WC_BlacksheetLxDVisAtt);
	else
		logic_WC_BlacksheetLxD->SetVisAttributes(G4VisAttributes::Invisible);
	
	
	
	
	
	//At this stage we should have the bottom layer containing the PMT's and blacksheet defined
	
	//Now I will rotate this volume by -90degrees on X axis to make minus plane (PMT's looking inward on -Y axis)
	const G4Transform3D rotation_LxD_minus = G4RotateX3D(-pi/2.);
	const G4Transform3D rotation_LxD_plus = G4RotateX3D(+pi/2.);
	
	
	
	//Now I will make the translation/reflection which will place theplus and minus layers on the +- y sides of the fiducial detector.
	
	G4Translate3D LDTranslate_minus(0., -(WC_MB_Fid_Width+WC_ActiveLayer_Depth)/2.,.0);		// for -y axis
	G4Translate3D LDTranslate_plus(0.,(WC_MB_Fid_Width+WC_ActiveLayer_Depth)/2, 0.);				//put on +y axis
	
	//ok, put up the sides!	
	// First side is on the negative axis--this just requires rotation and translation
	new G4PVPlacement(LDTranslate_minus*rotation_LxD_minus,    // rotate from xy plane to xz plane and put on -y axis--the PMT's should be looking inward
					  logic_WC_Active_LxD_minus,     // its logical volume				  
					  "LD_minus",        // its name
					  logic_WC_MB_tank_H20,      // its mother  volume, the WC water tank
					  false,           // no boolean operations
					  4);              // copy number 
	//Second side is on + axis
	new G4PVPlacement(LDTranslate_plus*rotation_LxD_plus,    // rotate from xy plane to xz plane and put on -y axis--the PMT's should be looking inward
					  logic_WC_Active_LxD_plus,     // its logical volume				  
					  "LD_plus",        // its name
					  logic_WC_MB_tank_H20,      // its mother  volume, the WC water tank
					  false,           // no boolean operations
					  5);              // copy number 											
	
	
	
	
	
	
	// Make sensitive detectors in the six faces
	
	G4SDManager* SDman = G4SDManager::GetSDMpointer();
	
	if (!aWCPMT) 
	{
	  aWCPMT = new WCSimWCSD( "/WCSim/glassFaceWCPMT", this );
		SDman->AddNewDetector( aWCPMT );
		G4cout << "Got to sensitive detector code \n";
	}
	logicGlassFaceWCPMT->SetSensitiveDetector( aWCPMT );
	
	
	//Put Fiducial into WaterTank, and WaterTank into "Cavern"
	new G4PVPlacement(0,G4ThreeVector(),logic_WC_MB_Fiducial,"physiMB_Fiducial",logic_WC_MB_tank_H20,false,0,false);// this should include now the fiducial volume and the pmt volumes around it.	
	
	new G4PVPlacement(0,G4ThreeVector(),logic_WC_MB_tank_H20,"physiMBTank",logic_WC_Cavern,false,0,false);
	
	
	//return the logical volume of the Cavern	
	
	return logic_WC_Cavern;   //whew!
}
//aah

G4LogicalVolume* WCSimDetectorConstruction::ConstructWC()
{

  //-----------------------------------------------------
  // Positions
  //-----------------------------------------------------

  debugMode = false;
  
  WCPosition=0.;//Set the WC tube offset to zero

  WCIDRadius = WCIDDiameter/2.;
  // the number of regular cell in phi direction:
  WCBarrelRingNPhi     = (G4int)(WCBarrelNumPMTHorizontal/WCPMTperCellHorizontal); 
  // the part of one ring, that is covered by the regular cells: 
  totalAngle  = 2.0*pi*rad*(WCBarrelRingNPhi*WCPMTperCellHorizontal/WCBarrelNumPMTHorizontal) ;
  // angle per regular cell:
  dPhi        =  totalAngle/ WCBarrelRingNPhi;
  // it's hight:
  barrelCellHeight  = (WCIDHeight-2.*WCBarrelPMTOffset)/WCBarrelNRings;
  // the hight of all regular cells together:
  mainAnnulusHeight = WCIDHeight -2.*WCBarrelPMTOffset -2.*barrelCellHeight;
  
  
//  innerAnnulusRadius = WCIDRadius - WCPMTExposeHeight-1.*mm;
  innerAnnulusRadius = WCIDRadius - fPMTConfigs[0].GetExposeHeight()-1.*mm;
  outerAnnulusRadius = WCIDRadius + WCBlackSheetThickness + 1.*mm;//+ Stealstructure etc.
  // the radii are measured to the center of the surfaces
  // (tangent distance). Thus distances between the corner and the center are bigger.
  WCLength    = WCIDHeight + 2*2.3*m;	//jl145 - reflects top veto blueprint, cf. Farshid Feyzi
  WCRadius    = (WCIDDiameter/2. + WCBlackSheetThickness + 1.5*m)/cos(dPhi/2.) ; // TODO: OD 
 
  // now we know the extend of the detector and are able to tune the tolerance
  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(WCLength > WCRadius ? WCLength : WCRadius);
  G4cout << "Computed tolerance = "
         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
         << " mm" << G4endl;

  //Decide if adding Gd
  water = "Water";
  if (WCAddGd)
  {water = "Doped Water";}


  //-----------------------------------------------------
  // Volumes
  //-----------------------------------------------------

  // The water barrel is placed in an tubs of air
  
  G4Tubs* solidWC = new G4Tubs("WC",
			       0.0*m,
			       WCRadius+2.*m, 
			       .5*WCLength+4.2*m,	//jl145 - per blueprint
			       0.*deg,
			       360.*deg);
  
  G4LogicalVolume* logicWC = 
    new G4LogicalVolume(solidWC,
			G4Material::GetMaterial("Air"),
			"WC",
			0,0,0);
 
 
   G4VisAttributes* showColor = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
   logicWC->SetVisAttributes(showColor);

   logicWC->SetVisAttributes(G4VisAttributes::Invisible); //amb79
  
  //-----------------------------------------------------
  // everything else is contained in this water tubs
  //-----------------------------------------------------
  G4Tubs* solidWCBarrel = new G4Tubs("WCBarrel",
				     0.0*m,
				     WCRadius+1.*m, // add a bit of extra space
				     .5*WCLength,  //jl145 - per blueprint
				     0.*deg,
				     360.*deg);
  
  G4LogicalVolume* logicWCBarrel = 
    new G4LogicalVolume(solidWCBarrel,
			G4Material::GetMaterial(water),
			"WCBarrel",
			0,0,0);

    G4VPhysicalVolume* physiWCBarrel = 
    new G4PVPlacement(0,
		      G4ThreeVector(0.,0.,0.),
		      logicWCBarrel,
		      "WCBarrel",
		      logicWC,
		      false,
	 	      0); 

// if(!debugMode)
//    logicWCBarrel->SetVisAttributes(G4VisAttributes::Invisible); 

  //-----------------------------------------------------
  // Form annular section of barrel to hold PMTs 
  //----------------------------------------------------

 
  G4double mainAnnulusZ[2] = {-mainAnnulusHeight/2., mainAnnulusHeight/2};
  G4double mainAnnulusRmin[2] = {innerAnnulusRadius, innerAnnulusRadius};
  G4double mainAnnulusRmax[2] = {outerAnnulusRadius, outerAnnulusRadius};

  G4Polyhedra* solidWCBarrelAnnulus = new G4Polyhedra("WCBarrelAnnulus",
                                                   0.*deg, // phi start
                                                   totalAngle, 
                                                   (G4int)WCBarrelRingNPhi, //NPhi-gon
                                                   2,
                                                   mainAnnulusZ,
                                                   mainAnnulusRmin,
                                                   mainAnnulusRmax);
  
  G4LogicalVolume* logicWCBarrelAnnulus = 
    new G4LogicalVolume(solidWCBarrelAnnulus,
			G4Material::GetMaterial(water),
			"WCBarrelAnnulus",
			0,0,0);
  // G4cout << *solidWCBarrelAnnulus << G4endl; 
  G4VPhysicalVolume* physiWCBarrelAnnulus = 
    new G4PVPlacement(0,
		      G4ThreeVector(0.,0.,0.),
		      logicWCBarrelAnnulus,
		      "WCBarrelAnnulus",
		      logicWCBarrel,
		      false,
		      0,true);
if(!debugMode)
   logicWCBarrelAnnulus->SetVisAttributes(G4VisAttributes::Invisible); //amb79
  //-----------------------------------------------------
  // Subdivide the BarrelAnnulus into rings
  //-----------------------------------------------------
  G4double RingZ[2] = {-barrelCellHeight/2.,
                        barrelCellHeight/2.};

  G4Polyhedra* solidWCBarrelRing = new G4Polyhedra("WCBarrelRing",
                                                   0.*deg,//+dPhi/2., // phi start
                                                   totalAngle, //phi end
                                                   (G4int)WCBarrelRingNPhi, //NPhi-gon
                                                   2,
                                                   RingZ,
                                                   mainAnnulusRmin,
                                                   mainAnnulusRmax);

  G4LogicalVolume* logicWCBarrelRing = 
    new G4LogicalVolume(solidWCBarrelRing,
			G4Material::GetMaterial(water),
			"WCBarrelRing",
			0,0,0);

  G4VPhysicalVolume* physiWCBarrelRing = 
    new G4PVReplica("WCBarrelRing",
		    logicWCBarrelRing,
		    logicWCBarrelAnnulus,
		    kZAxis,
		    (G4int)WCBarrelNRings-2,
		    barrelCellHeight);

if(!debugMode)
  logicWCBarrelRing->SetVisAttributes(G4VisAttributes::Invisible);
else {
        G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(0,0.5,1.));
        tmpVisAtt->SetForceWireframe(true);
        logicWCBarrelRing->SetVisAttributes(tmpVisAtt);
  }

  //-----------------------------------------------------
  // Subdivisions of the BarrelRings are cells
  //------------------------------------------------------


  G4Polyhedra* solidWCBarrelCell = new G4Polyhedra("WCBarrelCell",
                                                   -dPhi/2.+0.*deg, // phi start
                                                   dPhi, //total Phi
                                                   1, //NPhi-gon
                                                   2,
                                                   RingZ,
                                                   mainAnnulusRmin,
                                                   mainAnnulusRmax); 
  //G4cout << *solidWCBarrelCell << G4endl; 
  G4LogicalVolume* logicWCBarrelCell = 
    new G4LogicalVolume(solidWCBarrelCell,
			G4Material::GetMaterial(water),
			"WCBarrelCell",
			0,0,0);

  G4VPhysicalVolume* physiWCBarrelCell = 
    new G4PVReplica("WCBarrelCell",
		    logicWCBarrelCell,
		    logicWCBarrelRing,
		    kPhi,
		    (G4int)WCBarrelRingNPhi,
		    dPhi,
                    0.); 

  if(!debugMode)
  	logicWCBarrelCell->SetVisAttributes(G4VisAttributes::Invisible);
  else {
  	G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
  	tmpVisAtt->SetForceWireframe(true);
  	logicWCBarrelCell->SetVisAttributes(tmpVisAtt);
  }

  //-----------------------------------------------------------
  // The Blacksheet, a daughter of the cells containing PMTs,
  // and also some other volumes to make the edges light tight
  //-----------------------------------------------------------

  //-------------------------------------------------------------
  // add barrel blacksheet to the normal barrel cells 
  // ------------------------------------------------------------
  G4double annulusBlackSheetRmax[2] = {(WCIDRadius+WCBlackSheetThickness),
                                        WCIDRadius+WCBlackSheetThickness};
  G4double annulusBlackSheetRmin[2] = {(WCIDRadius),
                                        WCIDRadius};

  G4Polyhedra* solidWCBarrelCellBlackSheet = new G4Polyhedra("WCBarrelCellBlackSheet",
                                                   -dPhi/2., // phi start
                                                   dPhi, //total phi
                                                   1, //NPhi-gon
                                                   2,
                                                   RingZ,
                                                   annulusBlackSheetRmin,
                                                   annulusBlackSheetRmax);

  logicWCBarrelCellBlackSheet =
    new G4LogicalVolume(solidWCBarrelCellBlackSheet,
                        G4Material::GetMaterial("Blacksheet"),
                        "WCBarrelCellBlackSheet",
                          0,0,0);

   G4VPhysicalVolume* physiWCBarrelCellBlackSheet =
    new G4PVPlacement(0,
                      G4ThreeVector(0.,0.,0.),
                      logicWCBarrelCellBlackSheet,
                      "WCBarrelCellBlackSheet",
                      logicWCBarrelCell,
                      false,
                      0,true);

  G4LogicalBorderSurface * WaterBSBarrelCellSurface 
    = new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
                                 physiWCBarrelCell,
                                 physiWCBarrelCellBlackSheet, 
                                 OpWaterBSSurface);

 
   G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
      = new G4VisAttributes(G4Colour(0.2,0.9,0.2));
      if(debugMode)
        logicWCBarrelCellBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
      else
        logicWCBarrelCellBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);


 //-----------------------------------------------------------
 // add extra tower if nessecary
 // ---------------------------------------------------------
 
  // we have to declare the logical Volumes 
  // outside of the if block to access it later on 
  G4LogicalVolume* logicWCExtraTowerCell;
  G4LogicalVolume* logicWCExtraBorderCell;
  if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){

    // as the angles between the corners of the main annulus 
    // and the corners extra tower are different, we need to adjust the 
    // tangent distance the surfaces of the extra tower. Otherwise
    // the corners of the main annulus and the extra tower would 
    // not match. 
    G4double extraTowerRmin[2];
    G4double extraTowerRmax[2];
    for(int i = 0; i < 2 ; i++){
      extraTowerRmin[i] = mainAnnulusRmin[i] != 0 ? mainAnnulusRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
      extraTowerRmax[i] = mainAnnulusRmax[i] != 0 ? mainAnnulusRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
    }
    G4Polyhedra* solidWCExtraTower = new G4Polyhedra("WCextraTower",
  			 totalAngle-2.*pi,//+dPhi/2., // phi start
			 2.*pi -  totalAngle // total angle.
			 -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m),
			   // we need this little Gap between the extra tower and the main annulus
			   // to avoid a shared surface. Without the gap the photons stuck
			   // at this place for mare than 25 steps and the howl simulation
			   // crashes.
	         	 1, //NPhi-gon
		         2,
			 mainAnnulusZ,
			 extraTowerRmin,
		         extraTowerRmax);

    G4LogicalVolume* logicWCExtraTower = 
      new G4LogicalVolume(solidWCExtraTower,
			  G4Material::GetMaterial(water),
			  "WCExtraTower",
			  0,0,0);
    G4VPhysicalVolume* physiWCExtraTower = 
      new G4PVPlacement(0,
			G4ThreeVector(0.,0.,0.),
			logicWCExtraTower,
			"WCExtraTower",
			logicWCBarrel,
			false,
			0,true);
 

    logicWCExtraTower->SetVisAttributes(G4VisAttributes::Invisible);
  //-------------------------------------------
  // subdivide the extra tower into cells  
  //------------------------------------------

    G4Polyhedra* solidWCExtraTowerCell = new G4Polyhedra("WCExtraTowerCell",
			   totalAngle-2.*pi,//+dPhi/2., // phi start
			   2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //phi end
			   1, //NPhi-gon
			   2,
			   RingZ,
			   extraTowerRmin,
			   extraTowerRmax); 
    //G4cout << * solidWCExtraTowerCell << G4endl;
    logicWCExtraTowerCell = 
      new G4LogicalVolume(solidWCExtraTowerCell,
			  G4Material::GetMaterial(water),
			  "WCExtraTowerCell",
			  0,0,0);
    G4VPhysicalVolume* physiWCTowerCell = 
      new G4PVReplica("extraTowerCell",
		      logicWCExtraTowerCell,
		      logicWCExtraTower,
		      kZAxis,
		      (G4int)WCBarrelNRings-2,
		      barrelCellHeight);
    logicWCExtraTowerCell->SetVisAttributes(G4VisAttributes::Invisible);
    
    //---------------------------------------------
    // add blacksheet to this cells
    //--------------------------------------------

    G4double towerBSRmin[2];
    G4double towerBSRmax[2];
    for(int i = 0; i < 2; i++){
      towerBSRmin[i] = annulusBlackSheetRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
      towerBSRmax[i] = annulusBlackSheetRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
    }
    G4Polyhedra* solidWCTowerBlackSheet = new G4Polyhedra("WCExtraTowerBlackSheet",
			   totalAngle-2.*pi,//+dPhi/2., // phi start
			   2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //phi end
		           1, //NPhi-gon
			   2,
			   RingZ,
			   towerBSRmin,
			   towerBSRmax);
    //G4cout << * solidWCTowerBlackSheet << G4endl;
    logicWCTowerBlackSheet =
      new G4LogicalVolume(solidWCTowerBlackSheet,
			  G4Material::GetMaterial("Blacksheet"),
			  "WCExtraTowerBlackSheet",
			    0,0,0);

     G4VPhysicalVolume* physiWCTowerBlackSheet =
      new G4PVPlacement(0,
			G4ThreeVector(0.,0.,0.),
			logicWCTowerBlackSheet,
			"WCExtraTowerBlackSheet",
			logicWCExtraTowerCell,
			false,
			0,true);

    G4LogicalBorderSurface * WaterBSTowerCellSurface 
      = new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
				   physiWCTowerCell,
				   physiWCTowerBlackSheet, 
				   OpWaterBSSurface);

   
	if(debugMode)
	  logicWCTowerBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
	else
	  logicWCTowerBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);
}

  //jl145------------------------------------------------
  // Add top veto volume
  //-----------------------------------------------------

  G4bool WCTopVeto = (WCSimTuningParams->GetTopVeto());

  G4LogicalVolume* logicWCTopVeto;

  if(WCTopVeto){

	  G4double WCTyvekThickness = 1.0*mm; //completely made up

	  G4VSolid* solidWCTopVeto;
	  solidWCTopVeto =
			new G4Tubs(			"WCTopVeto",
								0.0*m,
								WCIDRadius + WCTyvekThickness,
								0.5*m + WCTyvekThickness,
								0.*deg,
								360.*deg);

	  logicWCTopVeto = 
			new G4LogicalVolume(solidWCTopVeto,
								G4Material::GetMaterial(water),
								"WCTopVeto",
								0,0,0);

	  G4VPhysicalVolume* physiWCTopVeto =
			new G4PVPlacement(	0,
								G4ThreeVector(0.,0.,WCIDHeight/2
													+1.0*m),
								logicWCTopVeto,
								"WCTopVeto",
								logicWCBarrel,
								false,0,true);

	  //Add the top veto Tyvek
	  //-----------------------------------------------------

	  G4VSolid* solidWCTVTyvek;
	  solidWCTVTyvek =
			new G4Tubs(			"WCTVTyvek",
								0.0*m,
								WCIDRadius,
								WCTyvekThickness/2,
								0.*deg,
								360.*deg);


	  G4LogicalVolume* logicWCTVTyvek =
			new G4LogicalVolume(solidWCTVTyvek,
								G4Material::GetMaterial("Tyvek"),
								"WCTVTyvek",
								0,0,0);

	  //Bottom
	  G4VPhysicalVolume* physiWCTVTyvekBot =
			new G4PVPlacement(	0,
		                  		G4ThreeVector(0.,0.,-0.5*m
													-WCTyvekThickness/2),
								logicWCTVTyvek,
		               			"WCTVTyvekBot",
		          				logicWCTopVeto,
				 				false,0,true);

	  G4LogicalBorderSurface * WaterTyTVSurfaceBot =
			new G4LogicalBorderSurface(	"WaterTyTVSurfaceBot",
										physiWCTopVeto,
										physiWCTVTyvekBot,
										OpWaterTySurface);

	  //Top
	  G4VPhysicalVolume* physiWCTVTyvekTop =
			new G4PVPlacement(	0,
		                  		G4ThreeVector(0.,0.,0.5*m
													+WCTyvekThickness/2),
								logicWCTVTyvek,
		               			"WCTVTyvekTop",
		          				logicWCTopVeto,
				 				false,0,true);

	  G4LogicalBorderSurface * WaterTyTVSurfaceTop =
			new G4LogicalBorderSurface(	"WaterTyTVSurfaceTop",
										physiWCTopVeto,
										physiWCTVTyvekTop,
										OpWaterTySurface);

	  //Side
	  G4VSolid* solidWCTVTyvekSide;
	  solidWCTVTyvekSide =
			new G4Tubs(			"WCTVTyvekSide",
								WCIDRadius,
								WCIDRadius + WCTyvekThickness,
								0.5*m + WCTyvekThickness,
								0.*deg,
								360.*deg);


	  G4LogicalVolume* logicWCTVTyvekSide =
			new G4LogicalVolume(solidWCTVTyvekSide,
								G4Material::GetMaterial("Tyvek"),
								"WCTVTyvekSide",
								0,0,0);

	  G4VPhysicalVolume* physiWCTVTyvekSide =
			new G4PVPlacement(	0,
		                  		G4ThreeVector(0.,0.,0.),
								logicWCTVTyvekSide,
		               			"WCTVTyvekSide",
		          				logicWCTopVeto,
				 				false,0,true);

	  G4LogicalBorderSurface * WaterTyTVSurfaceSide =
			new G4LogicalBorderSurface(	"WaterTyTVSurfaceSide",
										physiWCTopVeto,
										physiWCTVTyvekSide,
										OpWaterTySurface);

  }

  //
  //
  //jl145------------------------------------------------


  //////////// M Fechner : I need to  declare the PMT  here in order to
  // place the PMTs in the truncated cells
  //-----------------------------------------------------
  // The PMT
  //-----------------------------------------------------

  ////////////J Felde: The PMT logical volume is now constructed separately 
    //from any specific detector geometry so that both the Cylindrical and 
    //Mailbox geometries
  //  can use the same definition. 
  //K.Zbiri: The PMT volume and the PMT glass are now put in parallel. 
    //The PMT glass is the sensitive volume in this new configuration.

  ConstructPMT();

  

  //jl145------------------------------------------------
  // Add top veto PMTs
  //-----------------------------------------------------

  if(WCTopVeto){

	  G4double WCTVPMTSpacing = (WCSimTuningParams->GetTVSpacing())*cm;
	  G4double WCTVEdgeLimit = WCCapEdgeLimit;
	  G4int TVNCell = WCTVEdgeLimit/WCTVPMTSpacing + 2;

	  int icopy = 0;

	  for ( int i = -TVNCell ; i <  TVNCell; i++) {
		for (int j = -TVNCell ; j <  TVNCell; j++)   {

		  G4double xoffset = i*WCTVPMTSpacing + WCTVPMTSpacing*0.5;
		  G4double yoffset = j*WCTVPMTSpacing + WCTVPMTSpacing*0.5;

		  G4ThreeVector cellpos =
		  		G4ThreeVector(	xoffset, yoffset, -0.5*m);

//		  if ((sqrt(xoffset*xoffset + yoffset*yoffset) + WCPMTRadius) < WCTVEdgeLimit) {
		  if ((sqrt(xoffset*xoffset + yoffset*yoffset) + fPMTConfigs[0].GetRadius()) < WCTVEdgeLimit) {

		    G4VPhysicalVolume* physiCapPMT =
		    		new G4PVPlacement(	0,						// no rotation
		    							cellpos,				// its position
		    							logicWCPMT,				// its logical volume
		    							"WCPMT",				// its name 
		    							logicWCTopVeto,			// its mother volume
		    							false,					// no boolean os
		    							icopy);					// every PMT need a unique id.

		    icopy++;
		  }
		}
	  }

//	  G4double WCTVEfficiency = icopy*WCPMTRadius*WCPMTRadius/((WCIDRadius)*(WCIDRadius));
	  G4double WCTVEfficiency = icopy*fPMTConfigs[0].GetRadius()*fPMTConfigs[0].GetRadius()/((WCIDRadius)*(WCIDRadius));
	  G4cout << "Total on top veto: " << icopy << "\n";
	  G4cout << "Coverage was calculated to be: " << WCTVEfficiency << "\n";

  }

  //
  //
  //jl145------------------------------------------------


    ///////////////   Barrel PMT placement
  G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;
  WCPMTRotation->rotateY(90.*deg);

  G4double barrelCellWidth = 2.*WCIDRadius*tan(dPhi/2.);
  G4double horizontalSpacing   = barrelCellWidth/WCPMTperCellHorizontal;
  G4double verticalSpacing     = barrelCellHeight/WCPMTperCellVertical;

  for(G4double i = 0; i < WCPMTperCellHorizontal; i++){
    for(G4double j = 0; j < WCPMTperCellVertical; j++){
      G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius,
						 -barrelCellWidth/2.+(i+0.5)*horizontalSpacing,
						 -barrelCellHeight/2.+(j+0.5)*verticalSpacing);


      G4VPhysicalVolume* physiWCBarrelPMT =
	new G4PVPlacement(WCPMTRotation,              // its rotation
			  PMTPosition, 
			  logicWCPMT,                // its logical volume
			  "WCPMT",             // its name
			  logicWCBarrelCell,         // its mother volume
			  false,                     // no boolean operations
			  (int)(i*WCPMTperCellVertical+j),
			  true);                       
      
   // logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
     // daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
		// this is still the case.
    }
  }
  //-------------------------------------------------------------
  // Add PMTs in extra Tower if necessary
  //------------------------------------------------------------


  if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){

    G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;
    WCPMTRotation->rotateY(90.*deg);
    WCPMTRotation->rotateX((2*pi-totalAngle)/2.);//align the PMT with the Cell
                                                 
    G4double towerWidth = WCIDRadius*tan(2*pi-totalAngle);

    G4double horizontalSpacing   = towerWidth/(WCBarrelNumPMTHorizontal-WCBarrelRingNPhi*WCPMTperCellHorizontal);
    G4double verticalSpacing     = barrelCellHeight/WCPMTperCellVertical;

    for(G4double i = 0; i < (WCBarrelNumPMTHorizontal-WCBarrelRingNPhi*WCPMTperCellHorizontal); i++){
      for(G4double j = 0; j < WCPMTperCellVertical; j++){
	G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.),
				towerWidth/2.-(i+0.5)*horizontalSpacing,
			       -barrelCellHeight/2.+(j+0.5)*verticalSpacing);
	PMTPosition.rotateZ(-(2*pi-totalAngle)/2.); // align with the symmetry 
	                                            //axes of the cell 

	G4VPhysicalVolume* physiWCBarrelPMT =
	  new G4PVPlacement(WCPMTRotation,             // its rotation
			    PMTPosition, 
			    logicWCPMT,                // its logical volume
			    "WCPMT",             // its name
			    logicWCExtraTowerCell,         // its mother volume
			    false,                     // no boolean operations
			    (int)(i*WCPMTperCellVertical+j),
			    true);                       
	
		// logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
		// daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
		// this is still the case.
      }
    }

  }


  G4LogicalVolume* logicTopCapAssembly = ConstructCaps(-1);
  G4LogicalVolume* logicBottomCapAssembly = ConstructCaps(1);

  G4VPhysicalVolume* physiTopCapAssembly =
  new G4PVPlacement(0,
                  G4ThreeVector(0.,0.,(mainAnnulusHeight/2.+ capAssemblyHeight/2.)),
                  logicTopCapAssembly,
                  "TopCapAssembly",
                  logicWCBarrel,
                  false, 0,true);

  G4VPhysicalVolume* physiBottomCapAssembly =
    new G4PVPlacement(0,
                  G4ThreeVector(0.,0.,(-mainAnnulusHeight/2.- capAssemblyHeight/2.)),
                  logicBottomCapAssembly,
                  "BottomCapAssembly",
                  logicWCBarrel,
                  false, 0,true);


  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  if (!aWCPMT) 
  {
    aWCPMT = new WCSimWCSD( "/WCSim/glassFaceWCPMT",this );
    SDman->AddNewDetector( aWCPMT );
  }
  logicGlassFaceWCPMT->SetSensitiveDetector( aWCPMT );



  return logicWC;
}


G4LogicalVolume* WCSimDetectorConstruction::ConstructCaps(G4int zflip)
{

  capAssemblyHeight = (WCIDHeight-mainAnnulusHeight)/2+1*mm+WCBlackSheetThickness;

  G4Tubs* solidCapAssembly = new G4Tubs("CapAssembly",
							0.0*m,
							outerAnnulusRadius/cos(dPhi/2.), 
							capAssemblyHeight/2,
							0.*deg,
							360.*deg);

  G4LogicalVolume* logicCapAssembly =
    new G4LogicalVolume(solidCapAssembly,
                        G4Material::GetMaterial(water),
                        "CapAssembly",
                        0,0,0);




  //----------------------------------------------------
  // extra rings for the top and bottom of the annulus
  //---------------------------------------------------
  G4double borderAnnulusZ[3] = {-barrelCellHeight/2.*zflip,
                                (-barrelCellHeight/2.+(WCIDRadius-innerAnnulusRadius))*zflip,
				barrelCellHeight/2.*zflip};
  G4double borderAnnulusRmin[3] = { WCIDRadius, innerAnnulusRadius, innerAnnulusRadius};
  G4double borderAnnulusRmax[3] = {outerAnnulusRadius, outerAnnulusRadius,outerAnnulusRadius};
  G4Polyhedra* solidWCBarrelBorderRing = new G4Polyhedra("WCBarrelBorderRing",
                                                   0.*deg, // phi start
                                                   totalAngle,
                                                   (G4int)WCBarrelRingNPhi, //NPhi-gon
                                                   3,
                                                   borderAnnulusZ,
                                                   borderAnnulusRmin,
                                                   borderAnnulusRmax);
  G4LogicalVolume* logicWCBarrelBorderRing =
    new G4LogicalVolume(solidWCBarrelBorderRing,
                        G4Material::GetMaterial(water),
                        "WCBarrelRing",
                        0,0,0);
  //G4cout << *solidWCBarrelBorderRing << G4endl;

  G4VPhysicalVolume* physiWCBarrelBorderRing =
    new G4PVPlacement(0,
                  G4ThreeVector(0.,0.,(capAssemblyHeight/2.- barrelCellHeight/2.)*zflip),
                  logicWCBarrelBorderRing,
                  "WCBarrelBorderRing",
                  logicCapAssembly,
                  false, 0,true);


                  
  if(!debugMode) 
    logicWCBarrelBorderRing->SetVisAttributes(G4VisAttributes::Invisible); 
  //----------------------------------------------------
  // Subdevide border rings into cells
  // --------------------------------------------------
  G4Polyhedra* solidWCBarrelBorderCell = new G4Polyhedra("WCBarrelBorderCell",
                                                   -dPhi/2., // phi start
                                                   dPhi, //total angle 
                                                   1, //NPhi-gon
                                                   3,
                                                   borderAnnulusZ,
                                                   borderAnnulusRmin,
                                                   borderAnnulusRmax);

  G4LogicalVolume* logicWCBarrelBorderCell =
    new G4LogicalVolume(solidWCBarrelBorderCell, 
                        G4Material::GetMaterial(water),
                        "WCBarrelBorderCell", 
                        0,0,0);
  //G4cout << *solidWCBarrelBorderCell << G4endl;
  G4VPhysicalVolume* physiWCBarrelBorderCell =
    new G4PVReplica("WCBarrelBorderCell",
                    logicWCBarrelBorderCell,
                    logicWCBarrelBorderRing,
                    kPhi,
                    (G4int)WCBarrelRingNPhi,
                    dPhi,
                    0.);
  if(!debugMode)
        logicWCBarrelBorderCell->SetVisAttributes(G4VisAttributes::Invisible);
  else {
        G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
        tmpVisAtt->SetForceWireframe(true);
        logicWCBarrelBorderCell->SetVisAttributes(tmpVisAtt);
  }
  //------------------------------------------------------------
  // add blacksheet to the border cells.
  // We can use the same logical volume as for the normal
  // barrel cells.
  // ---------------------------------------------------------


   G4VPhysicalVolume* physiWCBarrelBorderCellBlackSheet =
    new G4PVPlacement(0,
                      G4ThreeVector(0.,0.,0.),
                      logicWCBarrelCellBlackSheet,
                      "WCBarrelCellBlackSheet",
                      logicWCBarrelBorderCell,
                      false,
                      0,true);

  G4LogicalBorderSurface * WaterBSBarrelBorderCellSurface
    = new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
                                 physiWCBarrelBorderCell,
                                 physiWCBarrelBorderCellBlackSheet,
                                 OpWaterBSSurface);

  // we have to declare the logical Volumes 
  // outside of the if block to access it later on 
  G4LogicalVolume* logicWCExtraTowerCell;
  G4LogicalVolume* logicWCExtraBorderCell;
  if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){
    //----------------------------------------------
    // also the extra tower need special cells at the 
    // top and th bottom.
    // (the top cell is created later on by reflecting the
    // bottom cell) 
    //---------------------------------------------
    G4double extraBorderRmin[3];
    G4double extraBorderRmax[3];
    for(int i = 0; i < 3; i++){
      extraBorderRmin[i] = borderAnnulusRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
      extraBorderRmax[i] = borderAnnulusRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
    } 
    G4Polyhedra* solidWCExtraBorderCell = new G4Polyhedra("WCspecialBarrelBorderCell",
			   totalAngle-2.*pi,//+dPhi/2., // phi start
			   2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //total phi
			   1, //NPhi-gon
			   3,
			   borderAnnulusZ,
			   extraBorderRmin,
			   extraBorderRmax);

    logicWCExtraBorderCell =
      new G4LogicalVolume(solidWCExtraBorderCell, 
			  G4Material::GetMaterial(water),
			  "WCspecialBarrelBorderCell", 
			  0,0,0);
    //G4cout << *solidWCExtraBorderCell << G4endl;

    G4VPhysicalVolume* physiWCExtraBorderCell =
      new G4PVPlacement(0,
                  G4ThreeVector(0.,0.,(capAssemblyHeight/2.- barrelCellHeight/2.)*zflip),
                  logicWCExtraBorderCell,
                  "WCExtraTowerBorderCell",
                  logicCapAssembly,
                  false, 0,true);

    logicWCExtraBorderCell->SetVisAttributes(G4VisAttributes::Invisible);

    G4VPhysicalVolume* physiWCExtraBorderBlackSheet =
      new G4PVPlacement(0,
			G4ThreeVector(0.,0.,0.),
			logicWCTowerBlackSheet,
			"WCExtraTowerBlackSheet",
			logicWCExtraBorderCell,
			false,
			0,true);

    G4LogicalBorderSurface * WaterBSExtraBorderCellSurface 
      = new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
				   physiWCExtraBorderCell,
				   physiWCExtraBorderBlackSheet, 
				   OpWaterBSSurface);

  }
 //------------------------------------------------------------
 // add caps
 // -----------------------------------------------------------
 
  G4double capZ[4] = { (-WCBlackSheetThickness-1.*mm)*zflip,
                      WCBarrelPMTOffset*zflip,
		      WCBarrelPMTOffset*zflip,
		      (WCBarrelPMTOffset+(WCIDRadius-innerAnnulusRadius))*zflip} ;
  G4double capRmin[4] = {  0. , 0., 0., 0.} ;
  G4double capRmax[4] = {outerAnnulusRadius, outerAnnulusRadius,  WCIDRadius, innerAnnulusRadius};
  G4VSolid* solidWCCap;
  if(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal){
    solidWCCap
      = new G4Polyhedra("WCCap",
			0.*deg, // phi start
			totalAngle, //phi end
			(int)WCBarrelRingNPhi, //NPhi-gon
			4, // 2 z-planes
			capZ, //position of the Z planes
			capRmin, // min radius at the z planes
			capRmax// max radius at the Z planes
			);
  } else {
  // if there is an extra tower, the cap volume is a union of
  // to polyhedra. We have to unite both parts, because there are 
  // PMTs that are on the border between both parts.
   G4Polyhedra* mainPart 
      = new G4Polyhedra("WCCapMainPart",
			0.*deg, // phi start
			totalAngle, //phi end
			(int)WCBarrelRingNPhi, //NPhi-gon
			4, // 2 z-planes
			capZ, //position of the Z planes
			capRmin, // min radius at the z planes
			capRmax// max radius at the Z planes
			);
    G4double extraCapRmin[4]; 
    G4double extraCapRmax[4]; 
    for(int i = 0; i < 4 ; i++){
      extraCapRmin[i] = capRmin[i] != 0. ?  capRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
      extraCapRmax[i] = capRmax[i] != 0. ? capRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
    }
    G4Polyhedra* extraSlice 
      = new G4Polyhedra("WCCapExtraSlice",
			totalAngle-2.*pi, // phi start
			2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //total phi 
			// fortunately there are no PMTs an the gap!
			1, //NPhi-gon
			4, //  z-planes
			capZ, //position of the Z planes
			extraCapRmin, // min radius at the z planes
			extraCapRmax// max radius at the Z planes
			);
     solidWCCap =
        new G4UnionSolid("WCCap", mainPart, extraSlice);

     //G4cout << *solidWCCap << G4endl;
   
  }
  // G4cout << *solidWCCap << G4endl;
  G4LogicalVolume* logicWCCap = 
    new G4LogicalVolume(solidWCCap,
			G4Material::GetMaterial(water),
			"WCCapPolygon",
			0,0,0);

  G4VPhysicalVolume* physiWCCap =
    new G4PVPlacement(0,                           // no rotation
		      G4ThreeVector(0.,0.,(-capAssemblyHeight/2.+1*mm+WCBlackSheetThickness)*zflip),     // its position
		      logicWCCap,          // its logical volume
		      "WCCap",             // its name
		      logicCapAssembly,                  // its mother volume
		      false,                       // no boolean operations
		      0,true);                          // Copy #


 
  if(debugMode){  
    G4VisAttributes* tmpVisAtt2 = new G4VisAttributes(G4Colour(.6,0.5,0.5));
   tmpVisAtt2->SetForceWireframe(true);
    logicWCCap->SetVisAttributes(tmpVisAtt2);
  } else
    logicWCCap->SetVisAttributes(G4VisAttributes::Invisible);


  //---------------------------------------------------------------------
  // add cap blacksheet
  // -------------------------------------------------------------------
  
  G4double capBlackSheetZ[4] = {-WCBlackSheetThickness*zflip, 0., 0., WCBarrelPMTOffset*zflip};
  G4double capBlackSheetRmin[4] = {0., 0., WCIDRadius, WCIDRadius};
  G4double capBlackSheetRmax[4] = {WCIDRadius+WCBlackSheetThickness, 
                                   WCIDRadius+WCBlackSheetThickness,
				   WCIDRadius+WCBlackSheetThickness,
				   WCIDRadius+WCBlackSheetThickness};
  G4VSolid* solidWCCapBlackSheet;
  if(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal){
    solidWCCapBlackSheet
      = new G4Polyhedra("WCCapBlackSheet",
			0.*deg, // phi start
			totalAngle, //total phi
			WCBarrelRingNPhi, //NPhi-gon
			4, //  z-planes
			capBlackSheetZ, //position of the Z planes
			capBlackSheetRmin, // min radius at the z planes
			capBlackSheetRmax// max radius at the Z planes
			);
    // G4cout << *solidWCCapBlackSheet << G4endl;
  } else { 
    // same as for the cap volume
     G4Polyhedra* mainPart
      = new G4Polyhedra("WCCapBlackSheetMainPart",
			0.*deg, // phi start
			totalAngle, //phi end
			WCBarrelRingNPhi, //NPhi-gon
			4, //  z-planes
			capBlackSheetZ, //position of the Z planes
			capBlackSheetRmin, // min radius at the z planes
			capBlackSheetRmax// max radius at the Z planes
			);
     G4double extraBSRmin[4];
     G4double extraBSRmax[4];
     for(int i = 0; i < 4 ; i++){
       extraBSRmin[i] = capBlackSheetRmin[i] != 0. ? capBlackSheetRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
       extraBSRmax[i] = capBlackSheetRmax[i] != 0. ? capBlackSheetRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
     }
     G4Polyhedra* extraSlice
      = new G4Polyhedra("WCCapBlackSheetextraSlice",
			totalAngle-2.*pi, // phi start
			2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //
			WCBarrelRingNPhi, //NPhi-gon
			4, //  z-planes
			capBlackSheetZ, //position of the Z planes
			extraBSRmin, // min radius at the z planes
			extraBSRmax// max radius at the Z planes
			);
    
     solidWCCapBlackSheet =
        new G4UnionSolid("WCCapBlackSheet", mainPart, extraSlice);
  }
  G4LogicalVolume* logicWCCapBlackSheet =
    new G4LogicalVolume(solidWCCapBlackSheet,
			G4Material::GetMaterial("Blacksheet"),
			"WCCapBlackSheet",
			0,0,0);
  G4VPhysicalVolume* physiWCCapBlackSheet =
    new G4PVPlacement(0,
                      G4ThreeVector(0.,0.,0.),
                      logicWCCapBlackSheet,
                      "WCCapBlackSheet",
                      logicWCCap,
                      false,
                      0,true);
   G4LogicalBorderSurface * WaterBSBottomCapSurface 
      = new G4LogicalBorderSurface("WaterBSCapPolySurface",
                                   physiWCCap,physiWCCapBlackSheet,
                                   OpWaterBSSurface);

   G4VisAttributes* WCCapBlackSheetVisAtt 
      = new G4VisAttributes(G4Colour(0.9,0.2,0.2));
    if(debugMode)
        logicWCCapBlackSheet->SetVisAttributes(WCCapBlackSheetVisAtt);
    else
        logicWCCapBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);


  //---------------------------------------------------------
  // Add top and bottom PMTs
  // -----------------------------------------------------
  
  G4double xoffset;
  G4double yoffset;
  G4int    icopy = 0;

  G4RotationMatrix* WCCapPMTRotation = new G4RotationMatrix;
  if(zflip==-1){
    WCCapPMTRotation->rotateY(180.*deg);
  }

  // loop over the cap
  G4int CapNCell = WCCapEdgeLimit/WCCapPMTSpacing + 2;
  for ( int i = -CapNCell ; i <  CapNCell; i++) {
    for (int j = -CapNCell ; j <  CapNCell; j++)   {

       
      xoffset = i*WCCapPMTSpacing + WCCapPMTSpacing*0.5;
      yoffset = j*WCCapPMTSpacing + WCCapPMTSpacing*0.5;

      
      G4ThreeVector cellpos = G4ThreeVector(xoffset, yoffset, 0);     
      //      G4double WCBarrelEffRadius = WCIDDiameter/2. - WCCapPMTSpacing;
      //      double comp = xoffset*xoffset + yoffset*yoffset 
      //	- 2.0 * WCBarrelEffRadius * sqrt(xoffset*xoffset+yoffset*yoffset)
      //	+ WCBarrelEffRadius*WCBarrelEffRadius;
      //      if ( (comp > WCPMTRadius*WCPMTRadius) && ((sqrt(xoffset*xoffset + yoffset*yoffset) + WCPMTRadius) < WCCapEdgeLimit) ) {
//            if (((sqrt(xoffset*xoffset + yoffset*yoffset) + WCPMTRadius) < WCCapEdgeLimit) ) {
            if (((sqrt(xoffset*xoffset + yoffset*yoffset) + fPMTConfigs[0].GetRadius()) < WCCapEdgeLimit) ) {


	G4VPhysicalVolume* physiCapPMT =
	  new G4PVPlacement(WCCapPMTRotation,
			    cellpos,                   // its position
			    logicWCPMT,                // its logical volume
			    "WCPMT", // its name 
			    logicWCCap,         // its mother volume
			    false,                 // no boolean os
			    icopy);               // every PMT need a unique id.
	
 // logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
    // daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
	// this is still the case.

	icopy++;
      }
    }
  }

  G4cout << "total on cap: " << icopy << "\n";
//  G4cout << "Coverage was calculated to be: " << (icopy*WCPMTRadius*WCPMTRadius/(WCIDRadius*WCIDRadius)) << "\n";
  G4cout << "Coverage was calculated to be: " << (icopy*fPMTConfigs[0].GetRadius()*fPMTConfigs[0].GetRadius()/(WCIDRadius*WCIDRadius)) << "\n";

    ///////////////   Barrel PMT placement
  G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;
  WCPMTRotation->rotateY(90.*deg);

  G4double barrelCellWidth = 2.*WCIDRadius*tan(dPhi/2.);
  G4double horizontalSpacing   = barrelCellWidth/WCPMTperCellHorizontal;
  G4double verticalSpacing     = barrelCellHeight/WCPMTperCellVertical;

  for(G4double i = 0; i < WCPMTperCellHorizontal; i++){
    for(G4double j = 0; j < WCPMTperCellVertical; j++){
      G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius,
						 -barrelCellWidth/2.+(i+0.5)*horizontalSpacing,
						 (-barrelCellHeight/2.+(j+0.5)*verticalSpacing)*zflip);

     G4VPhysicalVolume* physiWCBarrelBorderPMT =
	new G4PVPlacement(WCPMTRotation,                      // its rotation
			  PMTPosition,
			  logicWCPMT,                // its logical volume
			  "WCPMT",             // its name
			  logicWCBarrelBorderCell,         // its mother volume
			  false,                     // no boolean operations
			  (int)(i*WCPMTperCellVertical+j)
			  ,true);                      // no particular field


   // logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
     // daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
		// this is still the case.
    }
  }
  //-------------------------------------------------------------
  // Add PMTs in extra Tower if necessary
  //------------------------------------------------------------


  if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){

    G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;
    WCPMTRotation->rotateY(90.*deg);
    WCPMTRotation->rotateX((2*pi-totalAngle)/2.);//align the PMT with the Cell
                                                 
    G4double towerWidth = WCIDRadius*tan(2*pi-totalAngle);

    G4double horizontalSpacing   = towerWidth/(WCBarrelNumPMTHorizontal-WCBarrelRingNPhi*WCPMTperCellHorizontal);
    G4double verticalSpacing     = barrelCellHeight/WCPMTperCellVertical;

    for(G4double i = 0; i < (WCBarrelNumPMTHorizontal-WCBarrelRingNPhi*WCPMTperCellHorizontal); i++){
      for(G4double j = 0; j < WCPMTperCellVertical; j++){
	G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.),
				towerWidth/2.-(i+0.5)*horizontalSpacing,
			       (-barrelCellHeight/2.+(j+0.5)*verticalSpacing)*zflip);
	PMTPosition.rotateZ(-(2*pi-totalAngle)/2.); // align with the symmetry 
	                                            //axes of the cell 
	
	G4VPhysicalVolume* physiWCBarrelBorderPMT =
	  new G4PVPlacement(WCPMTRotation,                          // its rotation
			    PMTPosition,
			    logicWCPMT,                // its logical volume
			    "WCPMT",             // its name
			    logicWCExtraBorderCell,         // its mother volume
			    false,                     // no boolean operations
			    (int)(i*WCPMTperCellVertical+j)
			    ,true);                        // no particular field

		// logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
		// daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
		// this is still the case.
      }
    }

  }

return logicCapAssembly;

}


