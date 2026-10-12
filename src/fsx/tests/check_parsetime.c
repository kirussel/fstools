/*
 * Copyright (c) 2026 Kirk Russell. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

/*
 * Unit tests for parsetime(), using the Check framework
 * (https://libcheck.github.io/check/).  Run with "make check".
 */

#include <check.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "../parsetime.h"

START_TEST(test_bare_number)
{
	ck_assert_int_eq(parsetime("90"), 90);
	ck_assert_int_eq(parsetime("0"), 0);
	ck_assert_int_eq(parsetime("3600"), 3600);
}
END_TEST

START_TEST(test_each_unit)
{
	ck_assert_int_eq(parsetime("1s"), 1);
	ck_assert_int_eq(parsetime("1m"), 60);
	ck_assert_int_eq(parsetime("1h"), 3600);
	ck_assert_int_eq(parsetime("1d"), 86400);
	ck_assert_int_eq(parsetime("1w"), 604800);
	ck_assert_int_eq(parsetime("1y"), 31536000);
}
END_TEST

START_TEST(test_multi_digit)
{
	ck_assert_int_eq(parsetime("120s"), 120);
	ck_assert_int_eq(parsetime("45m"), 45 * 60);
	ck_assert_int_eq(parsetime("007s"), 7);
}
END_TEST

START_TEST(test_combined_units)
{
	ck_assert_int_eq(parsetime("1h30m"), 5400);
	ck_assert_int_eq(parsetime("1d2h3m4s"), 86400 + 2 * 3600 + 3 * 60 + 4);
	ck_assert_int_eq(parsetime("30m1h"), 5400);
	ck_assert_int_eq(parsetime("1m1m"), 120);
}
END_TEST

START_TEST(test_ignored_characters)
{
	/* A space between parts, or a stray trailing letter, is harmless. */
	ck_assert_int_eq(parsetime("1h 30m"), 5400);
	ck_assert_int_eq(parsetime("2x"), 2);
}
END_TEST

START_TEST(test_trailing_number_without_unit)
{
	/* Once a unit has been seen, trailing digits with no unit are dropped. */
	ck_assert_int_eq(parsetime("1m30"), 60);
}
END_TEST

START_TEST(test_zero_and_empty)
{
	ck_assert_int_eq(parsetime(""), 0);
	ck_assert_int_eq(parsetime("0s"), 0);
	ck_assert_int_eq(parsetime("0h0m"), 0);
	ck_assert_int_eq(parsetime("abc"), 0);
}
END_TEST

START_TEST(test_negative_falls_back_to_atoi)
{
	/* No unit was seen, so the bare-number fallback (atoi) applies. */
	ck_assert_int_eq(parsetime("-5"), -5);
}
END_TEST

START_TEST(test_number_like_atoi)
{
	/* Each number is read like atoi(): a sign counts, "." ends it. */
	ck_assert_int_eq(parsetime("1.5h"), 3600);
	ck_assert_int_eq(parsetime("-5m"), -300);
}
END_TEST

START_TEST(test_overflow)
{
	/* Values past INT_MAX are clamped to INT_MAX rather than wrapping. */
	ck_assert_int_eq(parsetime("99999999999"), INT_MAX);
	ck_assert_int_eq(parsetime("99999999999s"), INT_MAX);
	ck_assert_int_eq(parsetime("100y"), INT_MAX);
	ck_assert_int_eq(parsetime("68y1h"), 68 * 31536000 + 3600);
}
END_TEST

START_TEST(test_long_input)
{
	/* A long argument must not overflow any fixed-size buffer. */
	char buf[1024];

	memset(buf, '0', sizeof(buf));
	strcpy(buf + sizeof(buf) - 3, "5s");
	ck_assert_int_eq(parsetime(buf), 5);
}
END_TEST

static Suite *
parsetime_suite(void)
{
	Suite *s = suite_create("parsetime");
	TCase *tc = tcase_create("core");

	tcase_add_test(tc, test_bare_number);
	tcase_add_test(tc, test_each_unit);
	tcase_add_test(tc, test_multi_digit);
	tcase_add_test(tc, test_combined_units);
	tcase_add_test(tc, test_ignored_characters);
	tcase_add_test(tc, test_trailing_number_without_unit);
	tcase_add_test(tc, test_zero_and_empty);
	tcase_add_test(tc, test_negative_falls_back_to_atoi);
	tcase_add_test(tc, test_number_like_atoi);
	tcase_add_test(tc, test_overflow);
	tcase_add_test(tc, test_long_input);
	suite_add_tcase(s, tc);
	return s;
}

int
main(void)
{
	SRunner *sr = srunner_create(parsetime_suite());
	int failed;

	srunner_run_all(sr, CK_NORMAL);
	failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
