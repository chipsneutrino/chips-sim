"""Calculate scattering and absorption scales to get attenuation

Originally writter by Maciej Pfützner, modified by Josh Tingey
Email: j.tingey.16@ucl.ac.uk

This script calculates the tuning_parameter input for the CHIPS WCSim
detector simulation. It does this by calculating the scaling parameters
needed for a specified total attenuation length and relative contribution
factors
"""

import os
import argparse
import numpy as np

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True


def main():
    description = ('Calculate the scale factors for absorption and '
                   'Rayleigh scattering (to be used in tuning_parameters) '
                   'to get desired total attenuation (at a given wavelength). '
                   'Attenuation is calculated by scaling absorption and '
                   'scattering curves as coded in WCSim, taking into account '
                   'optional relative weights.')

    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('att_length', type=float, help='Desired total attenuation length in m')
    parser.add_argument('-w', '--wavelength', default=410, type=float,
                        help='Wavelength in nm at which attenuation is calculated (default 410)')
    parser.add_argument('-a', '--abs-weight', default=1, type=float,
                        help='Relative weight for absorption (default 1)')
    parser.add_argument('-s', '--scatt-weight', default=1, type=float,
                        help='Relative weight for Rayleigh scattering (default 1)')
    parser.add_argument('-m', '--mie-weight', default=-1, type=float,
                        help='Relative weight for Mie scattering (off when negative, default -1)')
    parser.add_argument('-v', '--verbosity', type=int, default=2, choices=[1, 2],
                        help='Verbosity: 1 - print just WCSim commands,\n2 - print everything')
    parser.add_argument('-p', '--plot', action='store_true',
                        help='Run attenuation.C to plot attenuation length curves')
    args = parser.parse_args()

    use_mie = True if args.mie_weight >= 0 else False

    if use_mie:
        scale_abs, scale_scat, scale_mie = get_scale_factors(args.att_length,
                                                             args.wavelength,
                                                             args.abs_weight,
                                                             args.scatt_weight,
                                                             args.mie_weight)
    else:
        scale_abs, scale_scat = get_scale_factors(args.att_length,
                                                  args.wavelength,
                                                  args.abs_weight,
                                                  args.scatt_weight)
        scale_mie = 0

    if args.verbosity == 2:
        print('Total attenuation length:', args.att_length,
              'm at', args.wavelength, 'nm wavelength')
        print('Absorption scale factor:', scale_abs)
        print('Scattering scale factor:', scale_scat)
        if use_mie:
            print('Mie scattering scale factor:', scale_mie)
        else:
            print('Mie scattering switched off (check WCSim code)')
        print('WCSim tuning_parameters.mac output:')
    print('/WCSim/tuning/abwff %.4g' % scale_abs)
    print('/WCSim/tuning/rayff %.4g' % scale_scat)
    print('/WCSim/tuning/mieff %.4g' % scale_mie)

    if args.plot:
        if use_mie:
            run_root_macro(scale_abs, scale_scat, scale_mie)
        else:
            run_root_macro(scale_abs, scale_scat, 1e6)


# Always return scales for absorption and rayleigh scattering
#  Return scale for mie scattering only if given mie_weight
def get_scale_factors(att_length, wavelength, abs_weight=1., scat_weight=1., mie_weight=-1):
    # get base attenuation give relative weights
    att_base = get_attenuation_length(wavelength, abs_weight, scat_weight, mie_weight)
    # now calculate total scale based on desired output
    print(str(att_base) + "_" + str(att_length))
    scale_total = att_length/att_base
    scale_abs = scale_total * abs_weight
    scale_scat = scale_total * scat_weight
    scale_mie = scale_total * mie_weight
    if mie_weight >= 0:
        result = (scale_abs, scale_scat, scale_mie)
    else:
        result = (scale_abs, scale_scat)
    return result


def get_attenuation_length(wavelength, scale_abs, scale_scat, scale_mie):
    wcsim_data = np.loadtxt('attenuation_wcsim.dat', unpack=True)
    (photon_e_array, abs_length_array, scat_length_array, mie_length_array) = wcsim_data

    photon_e = wavelength_to_energy(wavelength)
    abs_length = scale_abs*np.interp(photon_e, photon_e_array, abs_length_array)
    scat_length = scale_scat*np.interp(photon_e, photon_e_array, scat_length_array)
    mie_length = scale_mie*np.interp(photon_e, photon_e_array, mie_length_array)
    # if scale_mie negative, don't use it to compute total attenuation
    mie_inverse = 1./mie_length if scale_mie > 0 else 0.
    att_length = 1./(1./abs_length + 1./scat_length + mie_inverse)
    att_length /= 100.  # convert from cm to m

    return att_length


# return the attenuation length for a single effect (e.g. absorption only)
def get_single_length(wavelength, effect_name, scale):
    # load array: 0-energy, 1-abs, 2-ray scat, 3-mie scat
    wcsim_data = np.loadtxt('attenuation_wcsim.dat', unpack=True)
    photon_e_array = wcsim_data[0]

    effect_name = effect_name.lower()
    if 'abs' in effect_name:
        index = 1
    elif 'scat' in effect_name or 'ray' in effect_name:
        index = 2
    elif 'mie' in effect_name:
        index = 3
    else:
        print('Error! Unknown effect', effect_name)
        return

    length_array = wcsim_data[index]
    photon_e = wavelength_to_energy(wavelength)
    result = scale*np.interp(photon_e, photon_e_array, length_array)
    result /= 100.  # convert from cm to m

    return result


# convert photon wavelength in nm to energy in eV
def wavelength_to_energy(photon_wavelength):
    # from nm to m
    photon_wavelength /= 1e9
    # from wavelength in m to energy in eV
    photon_energy = ROOT.TMath.H()*ROOT.TMath.C()/(photon_wavelength*ROOT.TMath.Qe())
    return photon_energy


# convert photon energy in eV to wavelength in nm
def energy_to_wavelenght(photon_energy):
    # from energy in eV to wavelength in m
    photon_wavelength = ROOT.TMath.H()*ROOT.TMath.C()/(photon_energy*ROOT.TMath.Qe())
    # from m to nm
    photon_wavelength *= 1e9
    return photon_wavelength


# run root macro to plot result of scaling
def run_root_macro(scale_abs, scale_scat, scale_mie):
    os.system('root -l "attenuation.C(%g, %g, %g, true)"' % (scale_abs, scale_scat, scale_mie))


if __name__ == '__main__':
    main()
