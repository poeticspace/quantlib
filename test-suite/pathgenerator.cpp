/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005 StatPro Italia srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/reference/license.html>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include "pathgenerator.hpp"
#include "utilities.hpp"
#include <ql/montecarlo/mctraits.hpp>
#include <ql/processes/all.hpp>
#include <ql/daycounters/actual360.hpp>
#include <ql/quotes/simplequote.hpp>
#include <ql/utilities/dataformatters.hpp>

using namespace QuantLib;
using namespace boost::unit_test_framework;

QL_BEGIN_TEST_LOCALS(PathGeneratorTest)

void teardown() {
    Settings::instance().evaluationDate() = Date();
}

void testSingle(const boost::shared_ptr<StochasticProcess1D>& process,
                const std::string& tag, bool brownianBridge,
                Real expected, Real antithetic) {
    typedef PseudoRandom::rsg_type rsg_type;
    typedef PathGenerator<rsg_type>::sample_type sample_type;

    BigNatural seed = 42;
    Time length = 10;
    Size timeSteps = 12;
    rsg_type rsg = PseudoRandom::make_sequence_generator(timeSteps, seed);
    PathGenerator<rsg_type> generator(process, length, timeSteps,
                                      rsg, brownianBridge);
    Size i;
    for (i=0; i<100; i++)
        generator.next();

    sample_type sample = generator.next();
    Real calculated = sample.value.back();
    Real error = std::fabs(calculated-expected);
    Real tolerance = 2.0e-8;
    if (error > tolerance) {
        BOOST_ERROR("using " << tag << " process "
                    << (brownianBridge ? "with " : "without ")
                    << "brownian bridge:\n"
                    << std::setprecision(13)
                    << "    calculated: " << calculated << "\n"
                    << "    expected:   " << expected << "\n"
                    << "    error:      " << error << "\n"
                    << "    tolerance:  " << tolerance);
    }

    sample = generator.antithetic();
    calculated = sample.value.back();
    error = std::fabs(calculated-antithetic);
    tolerance = 2.0e-7;
    if (error > tolerance) {
        BOOST_ERROR("using " << tag << " process "
                    << (brownianBridge ? "with " : "without ")
                    << "brownian bridge:\n"
                    << "antithetic sample:\n"
                    << std::setprecision(13)
                    << "    calculated: " << calculated << "\n"
                    << "    expected:   " << antithetic << "\n"
                    << "    error:      " << error << "\n"
                    << "    tolerance:  " << tolerance);
    }

}

void testMultiple(const boost::shared_ptr<StochasticProcess>& process,
                  const std::string& tag, Real expected[], Real antithetic[]) {
    typedef PseudoRandom::rsg_type rsg_type;
    typedef MultiPathGenerator<rsg_type>::sample_type sample_type;

    BigNatural seed = 42;
    Time length = 10;
    Size timeSteps = 12;
    Size assets = process->size();
    rsg_type rsg = PseudoRandom::make_sequence_generator(timeSteps*assets,
                                                         seed);
    MultiPathGenerator<rsg_type> generator(process,
                                           TimeGrid(length, timeSteps),
                                           rsg, false);
    Size i, j;
    for (i=0; i<100; i++)
        generator.next();

    sample_type sample = generator.next();
    Array calculated(assets);
    Real error, tolerance = 2.0e-7;
    for (j=0; j<assets; j++)
        calculated[j] = sample.value[j].back();
    for (j=0; j<assets; j++) {
        error = std::fabs(calculated[j]-expected[j]);
        if (error > tolerance) {
            BOOST_ERROR("using " << tag << " process "
                        << "(" << io::ordinal(j+1) << " asset:)\n"
                        << std::setprecision(13)
                        << "    calculated: " << calculated[j] << "\n"
                        << "    expected:   " << expected[j] << "\n"
                        << "    error:      " << error << "\n"
                        << "    tolerance:  " << tolerance);
        }
    }

    sample = generator.antithetic();
    for (j=0; j<assets; j++)
        calculated[j] = sample.value[j].back();
    for (j=0; j<assets; j++) {
        error = std::fabs(calculated[j]-antithetic[j]);
        if (error > tolerance) {
            BOOST_ERROR("using " << tag << " process "
                        << "(" << io::ordinal(j+1) << " asset:)\n"
                        << "antithetic sample:\n"
                        << std::setprecision(13)
                        << "    calculated: " << calculated[j] << "\n"
                        << "    expected:   " << antithetic[j] << "\n"
                        << "    error:      " << error << "\n"
                        << "    tolerance:  " << tolerance);
        }
    }
}

QL_END_TEST_LOCALS(PathGeneratorTest)


void PathGeneratorTest::testPathGenerator() {

    BOOST_MESSAGE("Testing 1-D path generation against cached values...");

    QL_TEST_BEGIN

    Settings::instance().evaluationDate() = Date(26,April,2005);

    Handle<Quote> x0(boost::shared_ptr<Quote>(new SimpleQuote(100.0)));
    Handle<YieldTermStructure> r(flatRate(0.05, Actual360()));
    Handle<YieldTermStructure> q(flatRate(0.02, Actual360()));
    Handle<BlackVolTermStructure> sigma(flatVol(0.20, Actual360()));
    // commented values must be used when Halley's correction is enabled
    testSingle(boost::shared_ptr<StochasticProcess1D>(
                                 new BlackScholesMertonProcess(x0,q,r,sigma)),
               "Black-Scholes", false, 26.13784357783, 467.2928561411);
                                    // 26.13784357783, 467.2928562519);
    testSingle(boost::shared_ptr<StochasticProcess1D>(
                                 new BlackScholesMertonProcess(x0,q,r,sigma)),
               "Black-Scholes", true, 60.28215549393, 202.6143139999);
                                   // 60.28215551021, 202.6143139437);

    testSingle(boost::shared_ptr<StochasticProcess1D>(
                       new GeometricBrownianMotionProcess(100.0, 0.03, 0.20)),
               "geometric Brownian", false, 27.62223714065, 483.6026514084);
                                         // 27.62223714065, 483.602651493);

    testSingle(boost::shared_ptr<StochasticProcess1D>(
                                     new OrnsteinUhlenbeckProcess(0.1, 0.20)),
               "Ornstein-Uhlenbeck", false, -0.8372003433557, 0.8372003433557);

    testSingle(boost::shared_ptr<StochasticProcess1D>(
                                 new SquareRootProcess(0.1, 0.1, 0.20, 10.0)),
               "square-root", false, 1.70608664108, 6.024200546031);

    QL_TEST_TEARDOWN
}


void PathGeneratorTest::testMultiPathGenerator() {

    BOOST_MESSAGE("Testing n-D path generation against cached values...");

    QL_TEST_BEGIN

    Settings::instance().evaluationDate() = Date(26,April,2005);

    Handle<Quote> x0(boost::shared_ptr<Quote>(new SimpleQuote(100.0)));
    Handle<YieldTermStructure> r(flatRate(0.05, Actual360()));
    Handle<YieldTermStructure> q(flatRate(0.02, Actual360()));
    Handle<BlackVolTermStructure> sigma(flatVol(0.20, Actual360()));

    Matrix correlation(3,3);
    correlation[0][0] = 1.0; correlation[0][1] = 0.9; correlation[0][2] = 0.7;
    correlation[1][0] = 0.9; correlation[1][1] = 1.0; correlation[1][2] = 0.4;
    correlation[2][0] = 0.7; correlation[2][1] = 0.4; correlation[2][2] = 1.0;

    std::vector<boost::shared_ptr<StochasticProcess1D> > processes(3);
    boost::shared_ptr<StochasticProcess> process;

    processes[0] = boost::shared_ptr<StochasticProcess1D>(
                                 new BlackScholesMertonProcess(x0,q,r,sigma));
    processes[1] = boost::shared_ptr<StochasticProcess1D>(
                                 new BlackScholesMertonProcess(x0,q,r,sigma));
    processes[2] = boost::shared_ptr<StochasticProcess1D>(
                                 new BlackScholesMertonProcess(x0,q,r,sigma));
    process = boost::shared_ptr<StochasticProcess>(
                           new StochasticProcessArray(processes,correlation));
    // commented values must be used when Halley's correction is enabled
    Real result1[] = {
        188.2235868185,
        270.6713069569,
        113.0431145652 };
    // Real result1[] = {
    //     188.2235869273,
    //     270.6713071508,
    //     113.0431145652 };
    Real result1a[] = {
        64.89105742957,
        45.12494404804,
        108.0475146914 };
    // Real result1a[] = {
    //     64.89105739157,
    //     45.12494401537,
    //     108.0475146914 };
    testMultiple(process, "Black-Scholes", result1, result1a);

    processes[0] = boost::shared_ptr<StochasticProcess1D>(
                       new GeometricBrownianMotionProcess(100.0, 0.03, 0.20));
    processes[1] = boost::shared_ptr<StochasticProcess1D>(
                       new GeometricBrownianMotionProcess(100.0, 0.03, 0.20));
    processes[2] = boost::shared_ptr<StochasticProcess1D>(
                       new GeometricBrownianMotionProcess(100.0, 0.03, 0.20));
    process = boost::shared_ptr<StochasticProcess>(
                           new StochasticProcessArray(processes,correlation));
    Real result2[] = {
        174.8266131680,
        237.2692443633,
        119.1168555440 };
    // Real result2[] = {
    //     174.8266132344,
    //     237.2692444869,
    //     119.1168555605 };
    Real result2a[] = {
        57.69082393020,
        38.50016862915,
        116.4056510107 };
    // Real result2a[] = {
    //     57.69082387657,
    //     38.50016858691,
    //     116.4056510107 };
    testMultiple(process, "geometric Brownian", result2, result2a);

    processes[0] = boost::shared_ptr<StochasticProcess1D>(
                                     new OrnsteinUhlenbeckProcess(0.1, 0.20));
    processes[1] = boost::shared_ptr<StochasticProcess1D>(
                                     new OrnsteinUhlenbeckProcess(0.1, 0.20));
    processes[2] = boost::shared_ptr<StochasticProcess1D>(
                                     new OrnsteinUhlenbeckProcess(0.1, 0.20));
    process = boost::shared_ptr<StochasticProcess>(
                           new StochasticProcessArray(processes,correlation));
    Real result3[] = {
        0.2942058437284,
        0.5525006418386,
        0.02650931054575 };
    Real result3a[] = {
        -0.2942058437284,
        -0.5525006418386,
        -0.02650931054575 };
    testMultiple(process, "Ornstein-Uhlenbeck", result3, result3a);

    processes[0] = boost::shared_ptr<StochasticProcess1D>(
                                 new SquareRootProcess(0.1, 0.1, 0.20, 10.0));
    processes[1] = boost::shared_ptr<StochasticProcess1D>(
                                 new SquareRootProcess(0.1, 0.1, 0.20, 10.0));
    processes[2] = boost::shared_ptr<StochasticProcess1D>(
                                 new SquareRootProcess(0.1, 0.1, 0.20, 10.0));
    process = boost::shared_ptr<StochasticProcess>(
                           new StochasticProcessArray(processes,correlation));
    Real result4[] = {
        4.279510844897,
        4.943783503533,
        3.590930385958 };
    Real result4a[] = {
        2.763967737724,
        2.226487196647,
        3.503859264341 };
    testMultiple(process, "square-root", result4, result4a);

    QL_TEST_TEARDOWN
}


test_suite* PathGeneratorTest::suite() {
    test_suite* suite = BOOST_TEST_SUITE("Path generation tests");
    suite->add(BOOST_TEST_CASE(&PathGeneratorTest::testPathGenerator));
    suite->add(BOOST_TEST_CASE(&PathGeneratorTest::testMultiPathGenerator));
    return suite;
}

