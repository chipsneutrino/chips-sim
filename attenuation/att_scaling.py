"""
Calculate scattering and absorption scales to get attenuation

Run with -h option to see usage and description
"""

import sys
import argparse
import numpy as np

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
from ROOT import TMath


def usage():
    print 'Usage: python att_scaling.py <...>'


def main():
    description = ('Calculate the scale factors for absorption and '
                    'Rayleigh scattering (to be used in tuning_parameters) '
                    'to get desired total attenuation (at a given wavelength). '
                    'Attenuation is calculated by scaling absorption and '
                    'scattering curves as coded in WCSim, taking into account '
                    'optional relative weights.')
                    
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('att_length', type=float,
            help='Desired total attenuation length in m')
    parser.add_argument('-w', '--wavelength', default=410, type=float,
        help='Wavelength in nm at which attenuation is calculated (default 410)')
    parser.add_argument('-a', '--abs-weight', default=1, type=float,
            help='Relative weight for absorption (default 1)')
    parser.add_argument('-s', '--scatt-weight', default=1, type=float,
            help='Relative weight for scattering (default 1)')
    parser.add_argument('-v', '--verbosity', type=int, default=2, choices=[1, 2],
            help='Output verbosity: 1 - print just WCSim commands,\n'
            '2 - print everything (default)')
    args = parser.parse_args()
    
    scale_abs, scale_scat = get_scale_factors(args.att_length,
            args.wavelength, args.abs_weight, args.scatt_weight)

    if args.verbosity == 2:
        print 'Total attenuation length:', args.att_length, 
        print 'm at', args.wavelength, 'nm wavelength'
        print 'Absorption scale factor:', scale_abs
        print 'Scattering scale factor:', scale_scat
        print 
        print 'WCSim tuning_parameters.mac output:'
    print '/WCSim/tuning/abwff %.5f' % scale_abs
    print '/WCSim/tuning/rayff %.5f' % scale_scat


def get_scale_factors(att_length, wavelength, abs_weight=1., scat_weight=1.):
    # get base attenuation give relative weights
    att_base = get_attenuation_length(wavelength, abs_weight, scat_weight)
    # now calculate total scale based on desired output
    scale_total = att_length/att_base
    scale_abs = scale_total * abs_weight
    scale_scat = scale_total * scat_weight
    return (scale_abs, scale_scat)


def get_attenuation_length(wavelength, scale_abs, scale_scat):
    wcsim_data = np.loadtxt('attenuation_wcsim.dat', unpack=True)
    photon_e_array, abs_length_array, scat_length_array = wcsim_data
    
    photon_e = wavelength_to_energy(wavelength)
    abs_length = scale_abs*np.interp(photon_e, photon_e_array, abs_length_array)
    scat_length = scale_scat*np.interp(photon_e, photon_e_array, scat_length_array)
    att_length = 1./(1./abs_length + 1./scat_length)
    att_length /= 100. #convert from cm to m

    return att_length


#convert photon wavelength in nm to energy in eV
def wavelength_to_energy(photon_wavelength):
    # from nm to m
    photon_wavelength /= 1e9
    # from wavelength in m to energy in eV
    photon_energy = TMath.H()*TMath.C()/(photon_wavelength*TMath.Qe());
    return photon_energy

#convert photon energy in eV to wavelength in nm
def energy_to_wavelenght(photon_energy):
    # from energy in eV to wavelength in m
    photon_wavelength = TMath.H()*TMath.C()/(photon_energy*TMath.Qe());
    # from m to nm
    photon_wavelength *= 1e9
    return photon_wavelength


if __name__ == '__main__':
    main()



