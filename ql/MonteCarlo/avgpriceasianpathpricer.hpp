
/*
 * Copyright (C) 2000-2001 QuantLib Group
 *
 * This file is part of QuantLib.
 * QuantLib is a C++ open source library for financial quantitative
 * analysts and developers --- http://quantlib.org/
 *
 * QuantLib is free software and you are allowed to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of it under the conditions stated
 * in the QuantLib License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You should have received a copy of the license along with this file;
 * if not, please email quantlib-users@lists.sourceforge.net
 * The license is also available at http://quantlib.org/LICENSE.TXT
 *
 * The members of the QuantLib Group are listed in the Authors.txt file, also
 * available at http://quantlib.org/group.html
*/

/*! \file avgpriceasianpathpricer.hpp

    \fullpath
    Include/ql/MonteCarlo/%avgpriceasianpathpricer.hpp
    \brief path pricer for average price Asian option

*/

// $Id$
// $Log$
// Revision 1.2  2001/09/11 13:22:38  sadrejeb
// Monte Carlo modifications, cleaning up the merge of MonteCarloModel, and
// changed styleguide of typenames in pathpricer.
//
// Revision 1.1  2001/09/03 13:56:11  nando
// source (*.hpp and *.cpp) moved under topdir/ql
//
// Revision 1.13  2001/08/31 15:23:45  sigmud
// refining fullpath entries for doxygen documentation
//
// Revision 1.12  2001/08/21 14:21:22  nando
// removed default constructors and useless isInitialized_ private member
//
// [also enabled MS Visual C++ profiling]
//
// Revision 1.11  2001/08/09 14:59:46  sigmud
// header modification
//
// Revision 1.10  2001/08/08 11:07:48  sigmud
// inserting \fullpath for doxygen
//
// Revision 1.9  2001/08/07 11:25:54  sigmud
// copyright header maintenance
//
// Revision 1.8  2001/07/25 15:47:28  sigmud
// Change from quantlib.sourceforge.net to quantlib.org
//
// Revision 1.7  2001/07/19 16:40:10  lballabio
// Improved docs a bit
//
// Revision 1.6  2001/06/22 16:38:15  lballabio
// Improved documentation
//
// Revision 1.5  2001/05/24 15:38:08  nando
// smoothing #include xx.hpp and cutting old Log messages
//

#ifndef quantlib_montecarlo_average_price_asian_path_pricer_h
#define quantlib_montecarlo_average_price_asian_path_pricer_h

#include "ql/MonteCarlo/europeanpathpricer.hpp"

namespace QuantLib {

    namespace MonteCarlo {

        //! path pricer for average price Asian option
        class AveragePriceAsianPathPricer 
        : public EuropeanPathPricer {
          public:
            AveragePriceAsianPathPricer(Option::Type type,
                double underlying, double strike, double discount);
            virtual double operator()(const Path &path) const;
        };

    }

}


#endif
