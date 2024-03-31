// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef msr_airlib_RotorParams_hpp
#define msr_airlib_RotorParams_hpp

#include "common/Common.hpp"
#include <iostream>
#include <fstream>
#include <common/common_utils/json.hpp>
#include <direct.h>

namespace msr
{
namespace airlib
{

    //In NED system, +ve torque would generate clockwise rotation
    enum class RotorTurningDirection : int
    {
        RotorTurningDirectionCCW = -1,
        RotorTurningDirectionCW = 1
    };

    struct RotorParams
    {
        /*
            Ref: http://physics.stackexchange.com/a/32013/14061
            force in Newton = C_T * \rho * n^2 * D^4
            torque in N.m = C_P * \rho * n^2 * D^5 / (2*pi)
            where,
            \rho = air density (1.225 kg/m^3)
            n = revolutions per sec
            D = propeller diameter in meters
            C_T, C_P = dimensionless constants available at
            propeller performance database http://m-selig.ae.illinois.edu/props/propDB.html

            We use values for GWS 9X5 propeller for which,
            C_T = 0.109919, C_P = 0.040164 @ 6396.667 RPM
            */
        real_T C_T;
        //= 0.109919f; // the thrust co-efficient @ 6396.667 RPM, measured by UIUC.
        real_T C_P;
        //= 0.040164f; // the torque co-efficient at @ 6396.667 RPM, measured by UIUC.
        real_T air_density;
        //= 1.225f; //  kg/m^3
        real_T max_rpm;
        //= 6396.667f; // revolutions per minute
        real_T propeller_diameter;
        //= 0.2286f; //diameter in meters, default is for DJI Phantom 2
        real_T propeller_height;
        //= 1 / 100.0f; //height of cylindrical area when propeller rotates, 1 cm
        real_T control_signal_filter_tc;
        //= 0.005f; //time constant for low pass filter

        //height of rotor in meters
        real_T rotor_z;



        real_T revolutions_per_second;
        real_T max_speed; // in radians per second
        real_T max_speed_square;
        real_T max_thrust;
        //= 4.179446268f; //computed from above formula for the given constants
        real_T max_torque;
        //= 0.055562f; //computed from above formula


        void getParamsList()
        {
            FString pr_plugins_dir = FPaths::ProjectPluginsDir();
            FString rel_multirotor_params_path = TEXT("AirSim/multirotors/first/RotorParams.json"); 
            FString params_file_path = FPaths::Combine(pr_plugins_dir, rel_multirotor_params_path);
            std::string params_file_path_str(TCHAR_TO_UTF8(*params_file_path));
            std::ifstream file(params_file_path_str);
            if (!file.is_open()) {
                std::cerr << "Unable to open a file!" << std::endl;
            }

             /*Read JSON from file*/
            nlohmann::json jsonParams;
            file >> jsonParams;

            C_T = jsonParams["C_T"];
            //throw std::invalid_argument(std::to_string(C_T));
            C_P = jsonParams["C_P"];
            air_density = jsonParams["air_density"];
            max_rpm = jsonParams["max_rpm"];
            propeller_diameter = jsonParams["propeller_diameter"];
            propeller_height = jsonParams["propeller_height"];
            control_signal_filter_tc = jsonParams["control_signal_filter_tc"];
            max_thrust = jsonParams["max_thrust"];
            max_torque = jsonParams["max_torque"];
            rotor_z = jsonParams["rotor_z"] / 100;
            //UAirBlueprintLib::LogMessageString(std::to_string(params.C_T), "rotor params", LogDebugLevel::Failure);

        }


        // call this method to recalculate thrust if you want to use different numbers for C_T, C_P, max_rpm, etc.
        void calculateMaxThrust()
        {
            revolutions_per_second = max_rpm / 60;
            max_speed = revolutions_per_second * 2 * M_PIf; // radians / sec
            max_speed_square = pow(max_speed, 2.0f);

            real_T nsquared = revolutions_per_second * revolutions_per_second;
            max_thrust = C_T * air_density * nsquared * static_cast<real_T>(pow(propeller_diameter, 4));
            max_torque = C_P * air_density * nsquared * static_cast<real_T>(pow(propeller_diameter, 5)) / (2 * M_PIf);
        }
    };
}
} //namespace
#endif
