/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include "alluvialcorrection.h"
#include <sys/resource.h>


int main(int argc, char *argv[])
{
    if (argc < 3) {
        std :: cout << " Usage: ./AlluvialCorrection  inputBinaryFanFile inputIndexFile inputBasinFile \n";
        return 1;
    }
    const rlim_t kStackSize = 160 * 1024 * 1024;   // min stack size = 160 MB
    struct rlimit rl;
    int result;

    result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0) {
        if (rl.rlim_cur < kStackSize) {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0) {
                fprintf(stderr, "setrlimit returned result = %d\n", result);
              }
          }
      }

    AlluvialCorrection alv( argv[1], argv[2], argv[3] );
    alv.groupFan();
    alv.dilateFan();

    std :: cout << "Hello World!" << std :: endl;
    return 0;
}
