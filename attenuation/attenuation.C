Double_t energyToWavelength(Double_t energy)
{
    //get photon energy in eV
    //return wavelength in nm

    //conversion to m
    Double_t lambda = TMath::H()*TMath::C()/(energy*TMath::Qe());
    //to nm
    lambda *= 1e9;
    return lambda;
}

//convert attenuation length vs. photon energy
//  to att coefficient vs. photon wavelength (lambda)
TGraph *convertToCoeffLambda(TGraph *graph)
{
    //TGraph *graphConv = new TGraph(graph->GetN());
    TGraph *graphConv = (TGraph*)graph->Clone();
    for (int i = 0; i < graph->GetN(); i++) {
        double en, length;
        graph->GetPoint(i, en, length);
        double lambda = energyToWavelength(en);
        double coeff = 1./length;
        graphConv->SetPoint(i, lambda, coeff);
    }

    return graphConv;
}

//convert attenuation length vs. photon energy
//  to att length vs. photon wavelength (lambda)
TGraph *convertToLengthLambda(TGraph *graph)
{
    //TGraph *graphConv = new TGraph(graph->GetN());
    TGraph *graphConv = (TGraph*)graph->Clone();
    for (int i = 0; i < graph->GetN(); i++) {
        double en, length;
        graph->GetPoint(i, en, length);
        double lambda = energyToWavelength(en);
        graphConv->SetPoint(i, lambda, length);
    }

    return graphConv;
}

TGraph* addGraphs(TGraph *graph1, TGraph *graph2)
{
    Int_t numentries = graph1->GetN();
    TGraph *totalGraph = new TGraph(numentries);
    for (int i = 0; i < numentries; i++) {
        double x, y1, y2;
        graph1->GetPoint(i, x, y1);
        graph2->GetPoint(i, x, y2);
        totalGraph->SetPoint(i, x, y1 + y2);
    }
    return totalGraph;
}

TGraph* combineAbsRay(TGraph *graphAbs, TGraph *graphRay)
{
    Int_t numentries = graphAbs->GetN();
    TGraph *totalGraph = new TGraph(numentries);
    for (int i = 0; i < numentries; i++) {
        double en, abs, ray;
        graphAbs->GetPoint(i, en, abs);
        graphRay->GetPoint(i, en, ray);
        double total = 1./ (1./abs + 1./ray);
        totalGraph->SetPoint(i, en, total);
    }
    return totalGraph;
}

TGraph* combineAbsRayMie(TGraph *graphAbs, TGraph *graphRay, TGraph *graphMie)
{
    Int_t numentries = graphAbs->GetN();
    TGraph *totalGraph = new TGraph(numentries);
    for (int i = 0; i < numentries; i++) {
        double en, abs, ray, mie;
        graphAbs->GetPoint(i, en, abs);
        graphRay->GetPoint(i, en, ray);
        graphMie->GetPoint(i, en, mie);
        double total = 1./ (1./abs + 1./ray + 1./mie);
        totalGraph->SetPoint(i, en, total);
    }
    return totalGraph;
}


TGraph *getLeighsAtt()
{
    TGraph *graphLeigh = new TGraph(189);

    graphLeigh->SetPoint(0,6.21,1.42);
    graphLeigh->SetPoint(1,5.91,3.9);
    graphLeigh->SetPoint(2,5.64,6.34);
    graphLeigh->SetPoint(3,5.4,8.76);
    graphLeigh->SetPoint(4,4.97,14.16);
    graphLeigh->SetPoint(5,4.78,17.39);
    graphLeigh->SetPoint(6,4.6,23.31);
    graphLeigh->SetPoint(7,4.43,24.27);
    graphLeigh->SetPoint(8,4.28,34.36);
    graphLeigh->SetPoint(9,4.14,47.17);
    graphLeigh->SetPoint(10,4,51.55);
    graphLeigh->SetPoint(11,3.88,57.47);
    graphLeigh->SetPoint(12,3.27,87.9);
    graphLeigh->SetPoint(13,3.25,95.79);
    graphLeigh->SetPoint(14,3.22,106.27);
    graphLeigh->SetPoint(15,3.2,109.05);
    graphLeigh->SetPoint(16,3.18,117.51);
    graphLeigh->SetPoint(17,3.16,120.63);
    graphLeigh->SetPoint(18,3.14,123);
    graphLeigh->SetPoint(19,3.12,129.03);
    graphLeigh->SetPoint(20,3.1,150.83);
    graphLeigh->SetPoint(21,3.08,172.71);
    graphLeigh->SetPoint(22,3.07,188.68);
    graphLeigh->SetPoint(23,3.05,198.81);
    graphLeigh->SetPoint(24,3.03,211.42);
    graphLeigh->SetPoint(25,3.01,221.24);
    graphLeigh->SetPoint(26,2.99,225.23);
    graphLeigh->SetPoint(27,2.97,226.24);
    graphLeigh->SetPoint(28,2.96,220.26);
    graphLeigh->SetPoint(29,2.94,210.97);
    graphLeigh->SetPoint(30,2.92,209.21);
    graphLeigh->SetPoint(31,2.9,207.47);
    graphLeigh->SetPoint(32,2.89,202.02);
    graphLeigh->SetPoint(33,2.87,198.41);
    graphLeigh->SetPoint(34,2.85,188.68);
    graphLeigh->SetPoint(35,2.84,172.41);
    graphLeigh->SetPoint(36,2.82,157.48);
    graphLeigh->SetPoint(37,2.81,143.68);
    graphLeigh->SetPoint(38,2.79,133.16);
    graphLeigh->SetPoint(39,2.77,120.48);
    graphLeigh->SetPoint(40,2.76,108.46);
    graphLeigh->SetPoint(41,2.74,103.2);
    graphLeigh->SetPoint(42,2.73,103.95);
    graphLeigh->SetPoint(43,2.71,104.49);
    graphLeigh->SetPoint(44,2.7,102.15);
    graphLeigh->SetPoint(45,2.68,99.5);
    graphLeigh->SetPoint(46,2.67,98.91);
    graphLeigh->SetPoint(47,2.66,98.04);
    graphLeigh->SetPoint(48,2.64,94.34);
    graphLeigh->SetPoint(49,2.63,91.74);
    graphLeigh->SetPoint(50,2.61,87.72);
    graphLeigh->SetPoint(51,2.6,82.64);
    graphLeigh->SetPoint(52,2.59,78.74);
    graphLeigh->SetPoint(53,2.57,76.34);
    graphLeigh->SetPoint(54,2.56,73.53);
    graphLeigh->SetPoint(55,2.55,69.44);
    graphLeigh->SetPoint(56,2.53,66.67);
    graphLeigh->SetPoint(57,2.52,61.73);
    graphLeigh->SetPoint(58,2.51,57.8);
    graphLeigh->SetPoint(59,2.5,52.36);
    graphLeigh->SetPoint(60,2.48,49.02);
    graphLeigh->SetPoint(61,2.47,43.86);
    graphLeigh->SetPoint(62,2.46,39.06);
    graphLeigh->SetPoint(63,2.45,35.71);
    graphLeigh->SetPoint(64,2.43,30.77);
    graphLeigh->SetPoint(65,2.42,26.88);
    graphLeigh->SetPoint(66,2.41,25.25);
    graphLeigh->SetPoint(67,2.4,25.06);
    graphLeigh->SetPoint(68,2.39,24.45);
    graphLeigh->SetPoint(69,2.38,24.04);
    graphLeigh->SetPoint(70,2.36,23.98);
    graphLeigh->SetPoint(71,2.35,23.36);
    graphLeigh->SetPoint(72,2.34,23.04);
    graphLeigh->SetPoint(73,2.33,22.37);
    graphLeigh->SetPoint(74,2.32,22.12);
    graphLeigh->SetPoint(75,2.31,21.46);
    graphLeigh->SetPoint(76,2.3,21.1);
    graphLeigh->SetPoint(77,2.29,20.45);
    graphLeigh->SetPoint(78,2.28,19.57);
    graphLeigh->SetPoint(79,2.27,18.62);
    graphLeigh->SetPoint(80,2.26,17.7);
    graphLeigh->SetPoint(81,2.25,16.86);
    graphLeigh->SetPoint(82,2.24,16.78);
    graphLeigh->SetPoint(83,2.23,16.5);
    graphLeigh->SetPoint(84,2.22,16.16);
    graphLeigh->SetPoint(85,2.21,15.63);
    graphLeigh->SetPoint(86,2.2,15.58);
    graphLeigh->SetPoint(87,2.19,14.88);
    graphLeigh->SetPoint(88,2.18,14.39);
    graphLeigh->SetPoint(89,2.17,13.64);
    graphLeigh->SetPoint(90,2.16,12.95);
    graphLeigh->SetPoint(91,2.15,11.96);
    graphLeigh->SetPoint(92,2.14,11.16);
    graphLeigh->SetPoint(93,2.13,10.11);
    graphLeigh->SetPoint(94,2.12,9.09);
    graphLeigh->SetPoint(95,2.11,8.2);
    graphLeigh->SetPoint(96,2.1,7.4);
    graphLeigh->SetPoint(97,2.1,6.6);
    graphLeigh->SetPoint(98,2.09,5.98);
    graphLeigh->SetPoint(99,2.08,5.19);
    graphLeigh->SetPoint(100,2.07,4.5);
    graphLeigh->SetPoint(101,2.06,4.05);
    graphLeigh->SetPoint(102,2.05,3.88);
    graphLeigh->SetPoint(103,2.04,3.8);
    graphLeigh->SetPoint(104,2.04,3.78);
    graphLeigh->SetPoint(105,2.03,3.75);
    graphLeigh->SetPoint(106,2.02,3.73);
    graphLeigh->SetPoint(107,2.01,3.69);
    graphLeigh->SetPoint(108,2,3.63);
    graphLeigh->SetPoint(109,1.99,3.56);
    graphLeigh->SetPoint(110,1.99,3.53);
    graphLeigh->SetPoint(111,1.98,3.44);
    graphLeigh->SetPoint(112,1.97,3.43);
    graphLeigh->SetPoint(113,1.96,3.34);
    graphLeigh->SetPoint(114,1.96,3.32);
    graphLeigh->SetPoint(115,1.95,3.25);
    graphLeigh->SetPoint(116,1.94,3.22);
    graphLeigh->SetPoint(117,1.93,3.11);
    graphLeigh->SetPoint(118,1.92,3.08);
    graphLeigh->SetPoint(119,1.92,2.99);
    graphLeigh->SetPoint(120,1.91,2.94);
    graphLeigh->SetPoint(121,1.9,2.79);
    graphLeigh->SetPoint(122,1.9,2.7);
    graphLeigh->SetPoint(123,1.89,2.54);
    graphLeigh->SetPoint(124,1.88,2.44);
    graphLeigh->SetPoint(125,1.87,2.36);
    graphLeigh->SetPoint(126,1.87,2.33);
    graphLeigh->SetPoint(127,1.86,2.29);
    graphLeigh->SetPoint(128,1.85,2.28);
    graphLeigh->SetPoint(129,1.85,2.23);
    graphLeigh->SetPoint(130,1.84,2.23);
    graphLeigh->SetPoint(131,1.83,2.17);
    graphLeigh->SetPoint(132,1.83,2.15);
    graphLeigh->SetPoint(133,1.82,2.09);
    graphLeigh->SetPoint(134,1.81,2.06);
    graphLeigh->SetPoint(135,1.81,1.99);
    graphLeigh->SetPoint(136,1.8,1.94);
    graphLeigh->SetPoint(137,1.79,1.86);
    graphLeigh->SetPoint(138,1.79,1.79);
    graphLeigh->SetPoint(139,1.78,1.69);
    graphLeigh->SetPoint(140,1.77,1.6);
    graphLeigh->SetPoint(141,1.77,1.51);
    graphLeigh->SetPoint(142,1.76,1.42);
    graphLeigh->SetPoint(143,1.75,1.32);
    graphLeigh->SetPoint(144,1.75,1.21);
    graphLeigh->SetPoint(145,1.74,1.09);
    graphLeigh->SetPoint(146,1.74,0.99);
    graphLeigh->SetPoint(147,1.73,0.89);
    graphLeigh->SetPoint(148,1.72,0.81);
    graphLeigh->SetPoint(149,1.72,0.74);
    graphLeigh->SetPoint(150,1.71,0.67);
    graphLeigh->SetPoint(151,1.71,0.6);
    graphLeigh->SetPoint(152,1.705396,0.672495);
    graphLeigh->SetPoint(153,1.700724,0.6168651);
    graphLeigh->SetPoint(154,1.696077,0.5595345);
    graphLeigh->SetPoint(155,1.691456,0.5020836);
    graphLeigh->SetPoint(156,1.686859,0.4530217);
    graphLeigh->SetPoint(157,1.682288,0.4176761);
    graphLeigh->SetPoint(158,1.677741,0.3949603);
    graphLeigh->SetPoint(159,1.673219,0.3812283);
    graphLeigh->SetPoint(160,1.668721,0.3742095);
    graphLeigh->SetPoint(161,1.664247,0.3700825);
    graphLeigh->SetPoint(162,1.659798,0.3674309);
    graphLeigh->SetPoint(163,1.655371,0.3658447);
    graphLeigh->SetPoint(164,1.650969,0.3647904);
    graphLeigh->SetPoint(165,1.64659,0.3639275);
    graphLeigh->SetPoint(166,1.642234,0.3630818);
    graphLeigh->SetPoint(167,1.6379,0.3619516);
    graphLeigh->SetPoint(168,1.63359,0.3608805);
    graphLeigh->SetPoint(169,1.629303,0.3605813);
    graphLeigh->SetPoint(170,1.625037,0.3604643);
    graphLeigh->SetPoint(171,1.620794,0.3609978);
    graphLeigh->SetPoint(172,1.616574,0.3621876);
    graphLeigh->SetPoint(173,1.612375,0.3630818);
    graphLeigh->SetPoint(174,1.608198,0.3638745);
    graphLeigh->SetPoint(175,1.604042,0.3662333);
    graphLeigh->SetPoint(176,1.599908,0.3690445);
    graphLeigh->SetPoint(177,1.595795,0.3718025);
    graphLeigh->SetPoint(178,1.591703,0.3760812);
    graphLeigh->SetPoint(179,1.587632,0.3797661);
    graphLeigh->SetPoint(180,1.583582,0.3837004);
    graphLeigh->SetPoint(181,1.579553,0.3890748);
    graphLeigh->SetPoint(182,1.575544,0.3947109);
    graphLeigh->SetPoint(183,1.571555,0.4012197);
    graphLeigh->SetPoint(184,1.567587,0.40848);
    graphLeigh->SetPoint(185,1.563638,0.4152307);
    graphLeigh->SetPoint(186,1.559709,0.4211945);
    graphLeigh->SetPoint(187,1.5558,0.4285959);
    graphLeigh->SetPoint(188,1.551911,0.4360719);

    return graphLeigh;
}

TGraph* getWCSimAbs(Double_t ABWFF)
{
    const Int_t NUMENTRIES_water=60;

    Double_t GeV = 1e9; //in eV
    Double_t ENERGY_water[NUMENTRIES_water] =
    { 1.56962e-09*GeV, 1.58974e-09*GeV, 1.61039e-09*GeV, 1.63157e-09*GeV, 
        1.65333e-09*GeV, 1.67567e-09*GeV, 1.69863e-09*GeV, 1.72222e-09*GeV, 
        1.74647e-09*GeV, 1.77142e-09*GeV,1.7971e-09*GeV, 1.82352e-09*GeV, 
        1.85074e-09*GeV, 1.87878e-09*GeV, 1.90769e-09*GeV, 1.93749e-09*GeV, 
        1.96825e-09*GeV, 1.99999e-09*GeV, 2.03278e-09*GeV, 2.06666e-09*GeV,
        2.10169e-09*GeV, 2.13793e-09*GeV, 2.17543e-09*GeV, 2.21428e-09*GeV, 
        2.25454e-09*GeV, 2.29629e-09*GeV, 2.33962e-09*GeV, 2.38461e-09*GeV, 
        2.43137e-09*GeV, 2.47999e-09*GeV, 2.53061e-09*GeV, 2.58333e-09*GeV, 
        2.63829e-09*GeV, 2.69565e-09*GeV, 2.75555e-09*GeV, 2.81817e-09*GeV, 
        2.88371e-09*GeV, 2.95237e-09*GeV, 3.02438e-09*GeV, 3.09999e-09*GeV,
        3.17948e-09*GeV, 3.26315e-09*GeV, 3.35134e-09*GeV, 3.44444e-09*GeV, 
        3.54285e-09*GeV, 3.64705e-09*GeV, 3.75757e-09*GeV, 3.87499e-09*GeV, 
        3.99999e-09*GeV, 4.13332e-09*GeV, 4.27585e-09*GeV, 4.42856e-09*GeV, 
        4.59258e-09*GeV, 4.76922e-09*GeV, 4.95999e-09*GeV, 5.16665e-09*GeV, 
        5.39129e-09*GeV, 5.63635e-09*GeV, 5.90475e-09*GeV, 6.19998e-09*GeV };

    Double_t cm = 1e-2; //in m
    Double_t ABSORPTION_water[NUMENTRIES_water] =
    {
        16.1419*cm*ABWFF,  18.278*cm*ABWFF, 21.0657*cm*ABWFF, 24.8568*cm*ABWFF, 30.3117*cm*ABWFF, 
        38.8341*cm*ABWFF, 54.0231*cm*ABWFF, 81.2306*cm*ABWFF, 120.909*cm*ABWFF, 160.238*cm*ABWFF, 
        193.771*cm*ABWFF, 215.017*cm*ABWFF, 227.747*cm*ABWFF,  243.85*cm*ABWFF, 294.036*cm*ABWFF, 
        321.647*cm*ABWFF,  342.81*cm*ABWFF, 362.827*cm*ABWFF, 378.041*cm*ABWFF, 449.378*cm*ABWFF,
        739.434*cm*ABWFF, 1114.23*cm*ABWFF, 1435.56*cm*ABWFF, 1611.06*cm*ABWFF, 1764.18*cm*ABWFF, 
        2100.95*cm*ABWFF,  2292.9*cm*ABWFF, 2431.33*cm*ABWFF,  3053.6*cm*ABWFF, 4838.23*cm*ABWFF, 
        6539.65*cm*ABWFF, 7682.63*cm*ABWFF, 9137.28*cm*ABWFF, 12220.9*cm*ABWFF, 15270.7*cm*ABWFF, 
        19051.5*cm*ABWFF, 23671.3*cm*ABWFF, 29191.1*cm*ABWFF, 35567.9*cm*ABWFF,   42583*cm*ABWFF,
        49779.6*cm*ABWFF, 56465.3*cm*ABWFF,   61830*cm*ABWFF, 65174.6*cm*ABWFF, 66143.7*cm*ABWFF,   
        64820*cm*ABWFF,   61635*cm*ABWFF, 57176.2*cm*ABWFF, 52012.1*cm*ABWFF, 46595.7*cm*ABWFF, 
        41242.1*cm*ABWFF, 36146.3*cm*ABWFF, 31415.4*cm*ABWFF, 27097.8*cm*ABWFF, 23205.7*cm*ABWFF, 
        19730.3*cm*ABWFF, 16651.6*cm*ABWFF, 13943.6*cm*ABWFF, 11578.1*cm*ABWFF, 9526.13*cm*ABWFF
    };
    TGraph *graph = new TGraph(NUMENTRIES_water, ENERGY_water, ABSORPTION_water);
    return graph;
}

TGraph* getWCSimRay(Double_t RAYFF)
{
    const Int_t NUMENTRIES_water=60;

    Double_t GeV = 1e9; //in eV
    Double_t ENERGY_water[NUMENTRIES_water] =
    { 1.56962e-09*GeV, 1.58974e-09*GeV, 1.61039e-09*GeV, 1.63157e-09*GeV, 
        1.65333e-09*GeV, 1.67567e-09*GeV, 1.69863e-09*GeV, 1.72222e-09*GeV, 
        1.74647e-09*GeV, 1.77142e-09*GeV,1.7971e-09*GeV, 1.82352e-09*GeV, 
        1.85074e-09*GeV, 1.87878e-09*GeV, 1.90769e-09*GeV, 1.93749e-09*GeV, 
        1.96825e-09*GeV, 1.99999e-09*GeV, 2.03278e-09*GeV, 2.06666e-09*GeV,
        2.10169e-09*GeV, 2.13793e-09*GeV, 2.17543e-09*GeV, 2.21428e-09*GeV, 
        2.25454e-09*GeV, 2.29629e-09*GeV, 2.33962e-09*GeV, 2.38461e-09*GeV, 
        2.43137e-09*GeV, 2.47999e-09*GeV, 2.53061e-09*GeV, 2.58333e-09*GeV, 
        2.63829e-09*GeV, 2.69565e-09*GeV, 2.75555e-09*GeV, 2.81817e-09*GeV, 
        2.88371e-09*GeV, 2.95237e-09*GeV, 3.02438e-09*GeV, 3.09999e-09*GeV,
        3.17948e-09*GeV, 3.26315e-09*GeV, 3.35134e-09*GeV, 3.44444e-09*GeV, 
        3.54285e-09*GeV, 3.64705e-09*GeV, 3.75757e-09*GeV, 3.87499e-09*GeV, 
        3.99999e-09*GeV, 4.13332e-09*GeV, 4.27585e-09*GeV, 4.42856e-09*GeV, 
        4.59258e-09*GeV, 4.76922e-09*GeV, 4.95999e-09*GeV, 5.16665e-09*GeV, 
        5.39129e-09*GeV, 5.63635e-09*GeV, 5.90475e-09*GeV, 6.19998e-09*GeV };

    Double_t cm = 1e-2; //in m
    Double_t RAYLEIGH_water[NUMENTRIES_water] = {
        386929*cm*RAYFF,  366249*cm*RAYFF,  346398*cm*RAYFF,  327355*cm*RAYFF,  309097*cm*RAYFF,  
        291603*cm*RAYFF,  274853*cm*RAYFF,  258825*cm*RAYFF,  243500*cm*RAYFF,  228856*cm*RAYFF,  
        214873*cm*RAYFF,  201533*cm*RAYFF,  188816*cm*RAYFF,  176702*cm*RAYFF,  165173*cm*RAYFF,
        154210*cm*RAYFF,  143795*cm*RAYFF,  133910*cm*RAYFF,  124537*cm*RAYFF,  115659*cm*RAYFF,  
        107258*cm*RAYFF, 99318.2*cm*RAYFF, 91822.2*cm*RAYFF,   84754*cm*RAYFF, 78097.3*cm*RAYFF, 
        71836.5*cm*RAYFF,   65956*cm*RAYFF, 60440.6*cm*RAYFF, 55275.4*cm*RAYFF, 50445.6*cm*RAYFF,
        45937*cm*RAYFF, 41735.2*cm*RAYFF, 37826.6*cm*RAYFF, 34197.6*cm*RAYFF, 30834.9*cm*RAYFF, 
        27725.4*cm*RAYFF, 24856.6*cm*RAYFF, 22215.9*cm*RAYFF, 19791.3*cm*RAYFF, 17570.9*cm*RAYFF,   
        15543*cm*RAYFF, 13696.6*cm*RAYFF, 12020.5*cm*RAYFF, 10504.1*cm*RAYFF, 9137.15*cm*RAYFF,
        7909.45*cm*RAYFF,  6811.3*cm*RAYFF, 5833.25*cm*RAYFF,  4966.2*cm*RAYFF, 4201.36*cm*RAYFF, 
        3530.28*cm*RAYFF, 2944.84*cm*RAYFF, 2437.28*cm*RAYFF, 2000.18*cm*RAYFF,  1626.5*cm*RAYFF, 
        1309.55*cm*RAYFF, 1043.03*cm*RAYFF, 821.016*cm*RAYFF,  637.97*cm*RAYFF, 488.754*cm*RAYFF
    };

    TGraph *graph = new TGraph(NUMENTRIES_water, ENERGY_water, RAYLEIGH_water);
    return graph;
}

TGraph* getWCSimMie(Double_t MIEFF)
{
    const Int_t NUMENTRIES_water=60;

    Double_t GeV = 1e9; //in eV
    Double_t ENERGY_water[NUMENTRIES_water] =
    { 1.56962e-09*GeV, 1.58974e-09*GeV, 1.61039e-09*GeV, 1.63157e-09*GeV, 
        1.65333e-09*GeV, 1.67567e-09*GeV, 1.69863e-09*GeV, 1.72222e-09*GeV, 
        1.74647e-09*GeV, 1.77142e-09*GeV,1.7971e-09*GeV, 1.82352e-09*GeV, 
        1.85074e-09*GeV, 1.87878e-09*GeV, 1.90769e-09*GeV, 1.93749e-09*GeV, 
        1.96825e-09*GeV, 1.99999e-09*GeV, 2.03278e-09*GeV, 2.06666e-09*GeV,
        2.10169e-09*GeV, 2.13793e-09*GeV, 2.17543e-09*GeV, 2.21428e-09*GeV, 
        2.25454e-09*GeV, 2.29629e-09*GeV, 2.33962e-09*GeV, 2.38461e-09*GeV, 
        2.43137e-09*GeV, 2.47999e-09*GeV, 2.53061e-09*GeV, 2.58333e-09*GeV, 
        2.63829e-09*GeV, 2.69565e-09*GeV, 2.75555e-09*GeV, 2.81817e-09*GeV, 
        2.88371e-09*GeV, 2.95237e-09*GeV, 3.02438e-09*GeV, 3.09999e-09*GeV,
        3.17948e-09*GeV, 3.26315e-09*GeV, 3.35134e-09*GeV, 3.44444e-09*GeV, 
        3.54285e-09*GeV, 3.64705e-09*GeV, 3.75757e-09*GeV, 3.87499e-09*GeV, 
        3.99999e-09*GeV, 4.13332e-09*GeV, 4.27585e-09*GeV, 4.42856e-09*GeV, 
        4.59258e-09*GeV, 4.76922e-09*GeV, 4.95999e-09*GeV, 5.16665e-09*GeV, 
        5.39129e-09*GeV, 5.63635e-09*GeV, 5.90475e-09*GeV, 6.19998e-09*GeV };

    Double_t cm = 1e-2; //in m
    Double_t MIE_water[NUMENTRIES_water] = {
        7790020*cm*MIEFF, 7403010*cm*MIEFF, 7030610*cm*MIEFF, 6672440*cm*MIEFF, 6328120*cm*MIEFF, 
        5997320*cm*MIEFF, 5679650*cm*MIEFF, 5374770*cm*MIEFF, 5082340*cm*MIEFF, 4802000*cm*MIEFF, 
        4533420*cm*MIEFF, 4276280*cm*MIEFF, 4030220*cm*MIEFF, 3794950*cm*MIEFF, 3570120*cm*MIEFF,
        3355440*cm*MIEFF, 3150590*cm*MIEFF, 2955270*cm*MIEFF, 2769170*cm*MIEFF, 2592000*cm*MIEFF, 
        2423470*cm*MIEFF, 2263300*cm*MIEFF, 2111200*cm*MIEFF, 1966900*cm*MIEFF, 1830120*cm*MIEFF, 
        1700610*cm*MIEFF, 1578100*cm*MIEFF, 1462320*cm*MIEFF, 1353040*cm*MIEFF, 1250000*cm*MIEFF,
        1152960*cm*MIEFF, 1061680*cm*MIEFF,  975936*cm*MIEFF,  895491*cm*MIEFF,  820125*cm*MIEFF, 
        749619*cm*MIEFF,  683760*cm*MIEFF,  622339*cm*MIEFF,  565152*cm*MIEFF,  512000*cm*MIEFF, 
        462688*cm*MIEFF,  417027*cm*MIEFF,  374832*cm*MIEFF,  335923*cm*MIEFF,  300125*cm*MIEFF,
        267267*cm*MIEFF,  237184*cm*MIEFF,  209715*cm*MIEFF,  184704*cm*MIEFF,  162000*cm*MIEFF, 
        141456*cm*MIEFF,  122931*cm*MIEFF,  106288*cm*MIEFF, 91395.2*cm*MIEFF,   78125*cm*MIEFF, 
        66355.2*cm*MIEFF, 55968.2*cm*MIEFF, 46851.2*cm*MIEFF, 38896.2*cm*MIEFF,   32000*cm*MIEFF
    };

    TGraph *graph = new TGraph(NUMENTRIES_water, ENERGY_water, MIE_water);
    return graph;
}

void attenuation(double absWeight, double scatWeight,
    double mieWeight, bool onlyLambda=true)
{
    Double_t coeffAbs_nom = 1.00; //nominal value
    TGraph *absWCSim_nom = getWCSimAbs(coeffAbs_nom);
    absWCSim_nom->SetName("absWCSim_nom");
    absWCSim_nom->SetTitle("Absorption length vs. photon energy");
    absWCSim_nom->SetLineColor(kRed);
    absWCSim_nom->SetLineWidth(3);
    absWCSim_nom->SetLineStyle(2);
    absWCSim_nom->SetMarkerStyle(0);
    absWCSim_nom->SetMarkerSize(0.9);
    absWCSim_nom->SetMarkerColor(kRed);

    //Double_t coeffAbs_orig = 1.23; //original tuning_parameteers value
    //TGraph *absWCSim_orig = getWCSimAbs(coeffAbs_orig);
    //absWCSim_orig->SetName("absWCSim_orig");
    //absWCSim_orig->SetTitle("Absorption length vs. photon energy");
    //absWCSim_orig->SetLineColor(kRed);
    //absWCSim_orig->SetLineWidth(3);
    //absWCSim_orig->SetLineStyle(10);
    //absWCSim_orig->SetMarkerStyle(0);
    //absWCSim_orig->SetMarkerSize(0.9);
    //absWCSim_orig->SetMarkerColor(kRed);

    Double_t coeffAbs_cur = absWeight;
    TGraph *absWCSim_cur = getWCSimAbs(coeffAbs_cur);
    absWCSim_cur->SetName("absWCSim_cur");
    absWCSim_cur->SetTitle("Absorption length vs. photon energy");
    absWCSim_cur->SetLineColor(kRed);
    absWCSim_cur->SetLineWidth(3);
    absWCSim_cur->SetLineStyle(1);
    absWCSim_cur->SetMarkerStyle(0);
    absWCSim_cur->SetMarkerSize(0.9);
    absWCSim_cur->SetMarkerColor(kRed);


    Double_t coeffRay_nom = 1.0; //nominal value ?
    TGraph *rayWCSim_nom = getWCSimRay(coeffRay_nom);
    rayWCSim_nom->SetName("rayWCSim_nom");
    rayWCSim_nom->SetTitle("Scattering length vs. photon energy");
    rayWCSim_nom->SetLineColor(kBlue);
    rayWCSim_nom->SetLineWidth(3);
    rayWCSim_nom->SetLineStyle(2);
    rayWCSim_nom->SetMarkerStyle(0);
    rayWCSim_nom->SetMarkerSize(0.9);
    rayWCSim_nom->SetMarkerColor(kBlue);

    //Double_t coeffRay_orig = 0.8; //current value from tuning_parameters file
    //TGraph *rayWCSim_orig = getWCSimRay(coeffRay_orig);
    //rayWCSim_orig->SetName("rayWCSim_orig");
    //rayWCSim_orig->SetTitle("Scattering length vs. photon energy");
    //rayWCSim_orig->SetLineColor(kBlue);
    //rayWCSim_orig->SetLineWidth(3);
    //rayWCSim_orig->SetLineStyle(10);
    //rayWCSim_orig->SetMarkerStyle(0);
    //rayWCSim_orig->SetMarkerSize(0.9);
    //rayWCSim_orig->SetMarkerColor(kBlue);

    Double_t coeffRay_cur = scatWeight;
    TGraph *rayWCSim_cur = getWCSimRay(coeffRay_cur);
    rayWCSim_cur->SetName("rayWCSim_cur");
    rayWCSim_cur->SetTitle("Scattering length vs. photon energy");
    rayWCSim_cur->SetLineColor(kBlue);
    rayWCSim_cur->SetLineWidth(3);
    rayWCSim_cur->SetLineStyle(1);
    rayWCSim_cur->SetMarkerStyle(0);
    rayWCSim_cur->SetMarkerSize(0.9);
    rayWCSim_cur->SetMarkerColor(kBlue);

    //Unused
    //Double_t coeffRay_code = 0.625; //value from WCSim source code
    //TGraph *rayWCSim_code = getWCSimRay(coeffRay_code);
    //rayWCSim_code->SetName("rayWCSim_code");
    //rayWCSim_code->SetTitle("Scattering length vs. photon energy");
    //rayWCSim_code->SetLineColor(kBlue);
    //rayWCSim_code->SetLineWidth(3);
    //rayWCSim_code->SetLineStyle(2);
    //rayWCSim_code->SetMarkerStyle(0);
    //rayWCSim_code->SetMarkerSize(0.9);
    //rayWCSim_code->SetMarkerColor(kBlue);


    Double_t coeffMie_nom = 1.0; //nominal value
    TGraph *mieWCSim_nom = getWCSimMie(coeffMie_nom);
    mieWCSim_nom->SetName("mieWCSim_nom");
    mieWCSim_nom->SetTitle("Mie scattering length vs. photon energy");
    mieWCSim_nom->SetLineColor(kMagenta);
    mieWCSim_nom->SetLineWidth(3);
    mieWCSim_nom->SetLineStyle(2);
    mieWCSim_nom->SetMarkerStyle(0);
    mieWCSim_nom->SetMarkerSize(0.9);
    mieWCSim_nom->SetMarkerColor(kMagenta);

    Double_t coeffMie_cur = mieWeight;
    TGraph *mieWCSim_cur = getWCSimMie(coeffMie_cur);
    mieWCSim_cur->SetName("mieWCSim_cur");
    mieWCSim_cur->SetTitle("Mie scattering length vs. photon energy");
    mieWCSim_cur->SetLineColor(kMagenta);
    mieWCSim_cur->SetLineWidth(3);
    mieWCSim_cur->SetLineStyle(1);
    mieWCSim_cur->SetMarkerStyle(0);
    mieWCSim_cur->SetMarkerSize(0.9);
    mieWCSim_cur->SetMarkerColor(kMagenta);



    TGraph *totalWCSim_nom = combineAbsRayMie(absWCSim_nom, rayWCSim_nom, mieWCSim_nom);
    totalWCSim_nom->SetName("totalWCSim_nom");
    totalWCSim_nom->SetTitle("Total attenuation length vs. photon energy");
    totalWCSim_nom->SetLineColor(kGreen);
    totalWCSim_nom->SetLineWidth(3);
    totalWCSim_nom->SetLineStyle(2);
    totalWCSim_nom->SetMarkerStyle(0);
    totalWCSim_nom->SetMarkerSize(0.9);
    totalWCSim_nom->SetMarkerColor(kGreen);

    //TGraph *totalWCSim_orig = combineAbsRayMie(absWCSim_orig, rayWCSim_orig, mieWCSim_nom);
    //totalWCSim_orig->SetName("totalWCSim_orig");
    //totalWCSim_orig->SetTitle("Total attenuation length vs. photon energy");
    //totalWCSim_orig->SetLineColor(kGreen);
    //totalWCSim_orig->SetLineWidth(3);
    //totalWCSim_orig->SetLineStyle(10);
    //totalWCSim_orig->SetMarkerStyle(0);
    //totalWCSim_orig->SetMarkerSize(0.9);
    //totalWCSim_orig->SetMarkerColor(kGreen);

    TGraph *totalWCSim_cur = combineAbsRayMie(absWCSim_cur, rayWCSim_cur, mieWCSim_cur);
    totalWCSim_cur->SetName("totalWCSim_cur");
    totalWCSim_cur->SetTitle("Total attenuation length vs. photon energy");
    totalWCSim_cur->SetLineColor(kGreen);
    totalWCSim_cur->SetLineWidth(3);
    totalWCSim_cur->SetLineStyle(1);
    totalWCSim_cur->SetMarkerStyle(0);
    totalWCSim_cur->SetMarkerSize(0.9);
    totalWCSim_cur->SetMarkerColor(kGreen);



    TGraph *graphLeigh = getLeighsAtt();
    graphLeigh->SetName("graphLeigh");
    graphLeigh->SetTitle("Absorption length vs. photon energy");
    graphLeigh->SetFillColor(1);
    graphLeigh->SetMarkerStyle(8);
    graphLeigh->SetMarkerSize(1.0);

    TMultiGraph *mg = new TMultiGraph();
    mg->Add(graphLeigh);
    mg->Add(mieWCSim_nom);
    mg->Add(absWCSim_nom);
    mg->Add(rayWCSim_nom);
    mg->Add(totalWCSim_nom);
    //mg->Add(rayWCSim_code);
    //mg->Add(absWCSim_orig);
    //mg->Add(rayWCSim_orig);
    //mg->Add(totalWCSim_orig);
    mg->Add(absWCSim_cur);
    mg->Add(rayWCSim_cur);
    mg->Add(mieWCSim_cur);
    mg->Add(totalWCSim_cur);

    TLegend *leg = new TLegend(0.26, 0.13, 0.56, 0.40);
    leg->SetFillColor(kWhite);
    leg->SetBorderSize(0);
    leg->AddEntry(graphLeigh, "Leigh's attenuation data", "p");
    leg->AddEntry(absWCSim_nom, "WCSim coded absorption (1.0)", "l");
    leg->AddEntry(rayWCSim_nom, "WCSim coded scattering (1.0)", "l");
    leg->AddEntry(mieWCSim_nom, "WCSim coded Mie scattering (1.0)", "l");
    leg->AddEntry(totalWCSim_nom, "WCSim coded total", "l");
    //leg->AddEntry(absWCSim_orig, "WCSim tuned absorption (1.23)", "l");
    //leg->AddEntry(rayWCSim_orig, "WCSim tuned scattering (0.8)", "l");
    //leg->AddEntry(totalWCSim_orig, "WCSim tuned total", "l");
    leg->AddEntry(absWCSim_cur,
        Form("WCSim current absorption (%g)", absWeight), "l");
    leg->AddEntry(rayWCSim_cur,
        Form("WCSim current scattering (%g)", scatWeight), "l");
    leg->AddEntry(mieWCSim_cur,
        Form("WCSim current Mie scattering (%g)", mieWeight), "l");
    leg->AddEntry(totalWCSim_cur, "WCSim current total", "l");

    if (!onlyLambda) {
        TCanvas *can = new TCanvas("can", "length_energy",351,73,999,799);
        can->SetLogy();
        can->SetGridy();
        can->SetRightMargin(0.02);

        mg->Draw("A");
        mg->SetTitle("Water attenuation length vs. photon energy");
        mg->GetYaxis()->SetTitle("Attenuation length [m]");
        mg->GetXaxis()->SetTitle("Photon energy [eV]");
        mg->SetMinimum(0.05);
        mg->SetMaximum(1.0e3);
        mg->Draw("lp");

        leg->Draw();
    }

    //can->Print("../out/attenuation_nom.png");
    

    //---Now lets convert the graphs to attenuation length vs. photon wavelength

    TGraph *graphLeighConv = convertToLengthLambda(graphLeigh);

    TGraph *absWCSimConv_nom = convertToLengthLambda(absWCSim_nom);
    absWCSimConv_nom->SetName("absWCSimConv_nom");
    TGraph *rayWCSimConv_nom = convertToLengthLambda(rayWCSim_nom);
    rayWCSimConv_nom->SetName("rayWCSimConv_nom");
    TGraph *mieWCSimConv_nom = convertToLengthLambda(mieWCSim_nom);
    mieWCSimConv_nom->SetName("mieWCSimConv_nom");
    TGraph *totalWCSimConv_nom = convertToLengthLambda(totalWCSim_nom);
    totalWCSimConv_nom->SetName("totalWCSimConv_nom");

    //TGraph *absWCSimConv_orig = convertToLengthLambda(absWCSim_orig);
    //absWCSimConv_orig->SetName("absWCSimConv_orig");
    //TGraph *rayWCSimConv_orig = convertToLengthLambda(rayWCSim_orig);
    //rayWCSimConv_orig->SetName("rayWCSimConv_orig");
    //TGraph *totalWCSimConv_orig = convertToLengthLambda(totalWCSim_orig);
    //totalWCSimConv_orig->SetName("totalWCSimConv_orig");

    TGraph *absWCSimConv_cur = convertToLengthLambda(absWCSim_cur);
    absWCSimConv_cur->SetName("absWCSimConv_cur");
    TGraph *rayWCSimConv_cur = convertToLengthLambda(rayWCSim_cur);
    rayWCSimConv_cur->SetName("rayWCSimConv_cur");
    TGraph *mieWCSimConv_cur = convertToLengthLambda(mieWCSim_cur);
    mieWCSimConv_cur->SetName("mieWCSimConv_cur");
    TGraph *totalWCSimConv_cur = convertToLengthLambda(totalWCSim_cur);
    totalWCSimConv_cur->SetName("totalWCSimConv_cur");

    TMultiGraph *mg2 = new TMultiGraph();
    mg2->Add(graphLeighConv);
    mg2->Add(mieWCSimConv_nom);
    mg2->Add(absWCSimConv_nom);
    mg2->Add(rayWCSimConv_nom);
    mg2->Add(totalWCSimConv_nom);
    //mg2->Add(absWCSimConv_orig);
    //mg2->Add(rayWCSimConv_orig);
    //mg2->Add(totalWCSimConv_orig);
    mg2->Add(absWCSimConv_cur);
    mg2->Add(rayWCSimConv_cur);
    mg2->Add(mieWCSimConv_cur);
    mg2->Add(totalWCSimConv_cur);

    TCanvas *can2 = new TCanvas("can2", "length_lambda",351,73,999,799);
    can2->SetLogy();
    can2->SetGridy();
    can2->SetRightMargin(0.02);

    mg2->Draw("A");
    mg2->SetTitle("Water attenuation length vs. photon wavelength");
    mg2->GetYaxis()->SetTitle("Attenuation length [m]");
    mg2->GetXaxis()->SetTitle("Photon wavelength [nm]");
    mg2->SetMinimum(0.05);
    mg2->SetMaximum(1.0e3);
    mg2->Draw("lp");
    
    leg->Draw();
}


