/* SKaMPI   MPI-Benchmark

Copyright 2003-2008 Werner Augustin
Lehrstuhl Informatik fuer Naturwissenschaftler und Ingenieure 
Fakultaet fuer Informatik
University of Karlsruhe

2021 Camille Coti, Laboratoire d'Informatique de Paris Nord
Universite Sorbonne Paris Nord.

This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as
published by the Free Software Foundation

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA */

#ifndef SKAMPI_MPI
/* useful to avoid breaking some interfaces */
#define MPI_Comm int
#endif // SKAMPI_MPI

void init_globals(void);
void update_measurement_comm(MPI_Comm comm);

void set_send_buffer(char *buffer);
void set_recv_buffer(char *buffer);

void set_progname(char *s);
char *get_progname(void);

bool grootproc(void);
bool lrootproc(void); 

double wtime(void);
void finalize_ranks(void);

/*---------------------------------------------------------------------------*/
