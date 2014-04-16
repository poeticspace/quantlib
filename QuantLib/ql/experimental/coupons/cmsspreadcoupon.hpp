/*
 Copyright (C) 2014 Peter Caspers

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.


 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details. */

/*! \file cmsspreadcoupon.hpp
    \brief CMS spread coupon
*/

#ifndef quantlib_cmsspread_coupon_hpp
#define quantlib_cmsspread_coupon_hpp

#include <ql/cashflows/floatingratecoupon.hpp>
#include <ql/cashflows/capflooredcoupon.hpp>
#include <ql/time/schedule.hpp>

namespace QuantLib {

class SwapIndex;

//! CMS spread coupon class
/*! \warning This class does not perform any date adjustment,
             i.e., the start and end date passed upon construction
             should be already rolled to a business day.
*/
class CmsSpreadCoupon : public FloatingRateCoupon {
  public:
    CmsSpreadCoupon(const Date &paymentDate, Real nominal,
                    const Date &startDate, const Date &endDate,
                    Natural fixingDays,
                    const boost::shared_ptr<SwapIndex> &index1,
                    const boost::shared_ptr<SwapIndex> &index2,
                    Real gearing1 = 1.0, Real gearing2 = -1.0,
                    Spread spread = 0.0, const Date &refPeriodStart = Date(),
                    const Date &refPeriodEnd = Date(),
                    const DayCounter &dayCounter = DayCounter(),
                    bool isInArrears = false);
    //! \name Inspectors
    //@{
    const boost::shared_ptr<SwapSpreadIndex> &swapIndex1() const {
        return index1_;
    }
    const boost::shared_ptr<SwapSpreadIndex> &swapIndex2() const {
        return index2_;
    }
    //@}
    //! \name Visitability
    //@{
    virtual void accept(AcyclicVisitor &);
    //@}
  private:
    boost::shared_ptr<SwapIndex> index1_, index2_;
};

class CappedFlooredCmsSpreadCoupon : public CappedFlooredCoupon {
  public:
    CappedFlooredCmsSpreadCoupon(
        const Date &paymentDate, Real nominal, const Date &startDate,
        const Date &endDate, Natural fixingDays,
        const boost::shared_ptr<SwapIndex> &index1,
        const boost::shared_ptr<SwapIndex> &index2, Real gearing1 = 1.0,
        Real gearing2 = -1.0, Spread spread = 0.0,
        const Rate cap = Null<Rate>(), const Rate floor = Null<Rate>(),
        const Date &refPeriodStart = Date(), const Date &refPeriodEnd = Date(),
        const DayCounter &dayCounter = DayCounter(), bool isInArrears = false)
        : CappedFlooredCoupon(
              boost::shared_ptr<FloatingRateCoupon>(new CmsSpreadCoupon(
                  paymentDate, nominal, startDate, endDate, fixingDays, index1,
                  index2, gearing1, gearing2, spread, refPeriodStart,
                  refPeriodEnd, dayCounter, isInArrears)),
              cap, floor) {}

    virtual void accept(AcyclicVisitor &v) {
        Visitor<CappedFlooredCmsSpreadCoupon> *v1 =
            dynamic_cast<Visitor<CappedFlooredCmsSpreadCoupon> *>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            CappedFlooredCoupon::accept(v);
    }
};

//! helper class building a sequence of capped/floored cms-spread-rate coupons
class CmsSpreadLeg {
  public:
    CmsSpreadLeg(const Schedule &schedule,
                 const boost::shared_ptr<SwapSpreadIndex> &swapIndex1,
                 const boost::shared_ptr<SwapSpreadIndex> &swapIndex2,
                 Real gearing1, gearing2);
    CmsSpreadLeg &withNotionals(Real notional);
    CmsSpreadLeg &withNotionals(const std::vector<Real> &notionals);
    CmsSpreadLeg &withPaymentDayCounter(const DayCounter &);
    CmsSpreadLeg &withPaymentAdjustment(BusinessDayConvention);
    CmsSpreadLeg &withFixingDays(Natural fixingDays);
    CmsSpreadLeg &withFixingDays(const std::vector<Natural> &fixingDays);
    CmsSpreadLeg &withGearings(Real gearing1, Real gearing2);
    CmsSpreadLeg &withGearings(const std::vector<Real> &gearings1, const std::vector<Rel> &gearings2);
    CmsSpreadLeg &withSpreads(Spread spread);
    CmsSpreadLeg &withSpreads(const std::vector<Spread> &spreads);
    CmsSpreadLeg &withCaps(Rate cap);
    CmsSpreadLeg &withCaps(const std::vector<Rate> &caps);
    CmsSpreadLeg &withFloors(Rate floor);
    CmsSpreadLeg &withFloors(const std::vector<Rate> &floors);
    CmsSpreadLeg &inArrears(bool flag = true);
    CmsSpreadLeg &withZeroPayments(bool flag = true);
    operator Leg() const;

  private:
    Schedule schedule_;
    boost::shared_ptr<SwapIndex> swapIndex1_, swapIndex2_;
    std::vector<Real> notionals_;
    DayCounter paymentDayCounter_;
    BusinessDayConvention paymentAdjustment_;
    std::vector<Natural> fixingDays_;
    std::vector<Real> gearings1_, gearing2_;
    std::vector<Spread> spreads_;
    std::vector<Rate> caps_, floors_;
    bool inArrears_, zeroPayments_;
};
}

#endif
