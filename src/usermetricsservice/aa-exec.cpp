/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Pete Woods <pete.woods@canonical.com>
 */

#include <cerrno>
#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/apparmor.h>

using namespace std;

int main(int argc, char **argv) {
	if (argc < 3) {
		return 1;
	}

	string profile(argv[1]);

	// Shift off the first two arguments
	++argv; // program name
	++argv; // profile

	if (!profile.empty()) {
		if (aa_change_onexec(profile.c_str()) < 0) {
			switch (errno) {
			case ENOENT:
			case EACCES:
				cerr << "Profile does not exist" << endl;
				break;
			case EINVAL:
				cerr << "AppArmor interface not available" << endl;
				break;
			default:
				cerr << "Unknown error" << endl;
			}
			return 1;
		}
	}

	return execv(*argv, argv);
}
