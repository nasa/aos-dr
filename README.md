
## AOS-DR: Autonomy Operating System (AOS) Diagnostic Reasoner (DR)
-----------------------
### About
AOS-DR is software that performs runtime diagnosis of a system of interest. That is, it can determine when part of the system fails (fault/failure detection) and determine what part failed (fault/failure isolation). This software was first developed as part of the Autonomy Operating System project at NASA Ames Research Center. However, AOS-DR is general software that may be adapted to diagnose any system. The code and documentation will typically only use Diagnostic Reasoner, or DR, as the name of the software. \
\
The software is implemented as an application that runs within Core Flight Executive (cFE). cFE is open-source flight software created by NASA, and maintained separately [here](https://github.com/nasa/cFE). AOS-DR cannot run without being part of the cFE framework. It also currently uses another cFE app called [Limit Checker (LC)](https://github.com/nasa/LC) to classify data from the system into pass/fail results, which are the input to AOS-DR. cFE runs on Linux and VxWorks; however, AOS-DR has only been used on Linux.\
\
AOS-DR uses a dependency matrix (D-matrix) approach for performing diagnosis. The D-matrix is system-specific, and maps pass/fail results into system failure modes. A user of AOS-DR will need to create a D-matrix as well as several other tables in order to adapt it for their system. For more details on how AOS-DR works, please see the user manual in the doc directory.

-----------------------
### Contact
If you have questions about AOS-DR, please contact Adam Sweet <<adam.sweet@nasa.gov>> or Chris Tuebert <<christopher.a.teubert@nasa.gov>>.

-----------------------
### Contributing

Contributions to AOS-DR are welcome! Contributors will need to sign and submit a "Contributor License Agreement" (CLA), included in this source. 

-----------------------
### Copyright and Notices
The AOS-DR code is released under the NASA Open Source Agreement Version 1.3 license. A copy of the license is distributed with the source code.\
\
Copyright © 2020 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved.

Disclaimers:

No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."

Waiver and Indemnity:  RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.



