/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Ferdinando Ametrano

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/voltermstructures/interestratevolsurface.hpp>

namespace QuantLib {

    InterestRateVolSurface::InterestRateVolSurface(
                            const boost::shared_ptr<InterestRateIndex>& index,
                            BusinessDayConvention bdc,
                            const DayCounter& dc)
    : BlackVolSurface(bdc, dc), index_(index) {}

    InterestRateVolSurface::InterestRateVolSurface(
                            const boost::shared_ptr<InterestRateIndex>& index,
                            const Date& refDate,
                            const Calendar& cal,
                            BusinessDayConvention bdc,
                            const DayCounter& dc)
    : BlackVolSurface(refDate, cal, bdc, dc), index_(index) {}

    InterestRateVolSurface::InterestRateVolSurface(
                            const boost::shared_ptr<InterestRateIndex>& index,
                            Natural settlDays,
                            const Calendar& cal,
                            BusinessDayConvention bdc,
                            const DayCounter& dc)
    : BlackVolSurface(settlDays, cal, bdc, dc), index_(index) {}

    void InterestRateVolSurface::accept(AcyclicVisitor& v) {
        Visitor<InterestRateVolSurface>* v1 =
            dynamic_cast<Visitor<InterestRateVolSurface>*>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            BlackVolSurface::accept(v);
    }

}

