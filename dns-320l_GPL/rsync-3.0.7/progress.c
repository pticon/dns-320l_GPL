/*
 * Routines to output progress information during a file transfer.
 *
 * Copyright (C) 1996-2000 Andrew Tridgell
 * Copyright (C) 1996 Paul Mackerras
 * Copyright (C) 2001, 2002 Martin Pool <mbp@samba.org>
 * Copyright (C) 2003-2009 Wayne Davison
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, visit the http://fsf.org website.
 */

#include "rsync.h"

//royc 20100603
#define	COUNT_MAX_NUM		300
#define PROGRESS_INTERVAL	4
#define PROGRESS_NUM		(100/PROGRESS_INTERVAL)
#define MIN_TOTAL_SIZE		32768

extern char tmp_file_name[256];
extern FILE *tmp_fp;
extern int do_count_progress;
extern int table_flag;
extern int64 pg_table[PROGRESS_NUM+1];
extern int count;
extern int percent;
//for rsync system define
extern int verbose;
//royc end

extern int am_server;
extern int need_unsorted_flist;
extern struct stats stats;
extern struct file_list *cur_flist;

#define PROGRESS_HISTORY_SECS 5

#ifdef GETPGRP_VOID
#define GETPGRP_ARG
#else
#define GETPGRP_ARG 0
#endif

struct progress_history {
	struct timeval time;
	OFF_T ofs;
};

int progress_is_active = 0;

static struct progress_history ph_start;
static struct progress_history ph_list[PROGRESS_HISTORY_SECS];
static int newest_hpos, oldest_hpos;
static int current_file_index;

static unsigned long msdiff(struct timeval *t1, struct timeval *t2)
{
	return (t2->tv_sec - t1->tv_sec) * 1000L
	     + (t2->tv_usec - t1->tv_usec) / 1000;
}


/**
 * @param ofs Current position in file
 * @param size Total size of file
 * @param is_last True if this is the last time progress will be
 * printed for this file, so we should output a newline.  (Not
 * necessarily the same as all bytes being received.)
 **/
static void rprint_progress(OFF_T ofs, OFF_T size, struct timeval *now,
			    int is_last)
{
	char rembuf[64], eol[128];
	const char *units;
	int pct = ofs == size ? 100 : (int) (100.0 * ofs / size);
	unsigned long diff;
	double rate, remain;

	if (is_last) {
		/* Compute stats based on the starting info. */
		if (!ph_start.time.tv_sec
		    || !(diff = msdiff(&ph_start.time, now)))
			diff = 1;
		rate = (double) (ofs - ph_start.ofs) * 1000.0 / diff / 1024.0;
		/* Switch to total time taken for our last update. */
		remain = (double) diff / 1000.0;
	} else {
		/* Compute stats based on recent progress. */
		if (!(diff = msdiff(&ph_list[oldest_hpos].time, now)))
			diff = 1;
		rate = (double) (ofs - ph_list[oldest_hpos].ofs) * 1000.0
		     / diff / 1024.0;
		remain = rate ? (double) (size - ofs) / rate / 1000.0 : 0.0;
	}

	if (rate > 1024*1024) {
		rate /= 1024.0 * 1024.0;
		units = "GB/s";
	} else if (rate > 1024) {
		rate /= 1024.0;
		units = "MB/s";
	} else {
		units = "kB/s";
	}

	if (remain < 0)
		strlcpy(rembuf, "  ??:??:??", sizeof rembuf);
	else {
		snprintf(rembuf, sizeof rembuf, "%4d:%02d:%02d",
			 (int) (remain / 3600.0),
			 (int) (remain / 60.0) % 60,
			 (int) remain % 60);
	}

	if (is_last) {
		snprintf(eol, sizeof eol, " (xfer#%d, to-check=%d/%d)\n",
			stats.num_transferred_files,
			stats.num_files - current_file_index - 1,
			stats.num_files);
	} else
		strlcpy(eol, "\r", sizeof eol);
	progress_is_active = 0;
	rprintf(FCLIENT, "%12s %3d%% %7.2f%s %s%s",
		human_num(ofs), pct, rate, units, rembuf, eol);
	if (!is_last)
		progress_is_active = 1;
}

void set_current_file_index(struct file_struct *file, int ndx)
{
	if (need_unsorted_flist)
		current_file_index = flist_find(cur_flist, file) + cur_flist->ndx_start;
	else
		current_file_index = ndx;
	current_file_index -= cur_flist->flist_num;
}

void end_progress(OFF_T size)
{
	if (!am_server) {
		struct timeval now;
		gettimeofday(&now, NULL);
		rprint_progress(size, size, &now, True);
	}
	memset(&ph_start, 0, sizeof ph_start);
}

void show_progress(OFF_T ofs, OFF_T size)
{
	struct timeval now;
#if defined HAVE_GETPGRP && defined HAVE_TCGETPGRP
	static pid_t pgrp = -1;
	pid_t tc_pgrp;
#endif

	if (am_server)
		return;

#if defined HAVE_GETPGRP && defined HAVE_TCGETPGRP
	if (pgrp == -1)
		pgrp = getpgrp(GETPGRP_ARG);
#endif

	gettimeofday(&now, NULL);

	if (!ph_start.time.tv_sec) {
		int i;

		/* Try to guess the real starting time when the sender started
		 * to send us data by using the time we last received some data
		 * in the last file (if it was recent enough). */
		if (msdiff(&ph_list[newest_hpos].time, &now) <= 1500) {
			ph_start.time = ph_list[newest_hpos].time;
			ph_start.ofs = 0;
		} else {
			ph_start.time.tv_sec = now.tv_sec;
			ph_start.time.tv_usec = now.tv_usec;
			ph_start.ofs = ofs;
		}

		for (i = 0; i < PROGRESS_HISTORY_SECS; i++)
			ph_list[i] = ph_start;
	}
	else {
		if (msdiff(&ph_list[newest_hpos].time, &now) < 1000)
			return;

		newest_hpos = oldest_hpos;
		oldest_hpos = (oldest_hpos + 1) % PROGRESS_HISTORY_SECS;
		ph_list[newest_hpos].time.tv_sec = now.tv_sec;
		ph_list[newest_hpos].time.tv_usec = now.tv_usec;
		ph_list[newest_hpos].ofs = ofs;
	}

#if defined HAVE_GETPGRP && defined HAVE_TCGETPGRP
	tc_pgrp = tcgetpgrp(STDOUT_FILENO);
	if (tc_pgrp != pgrp && tc_pgrp != -1)
		return;
#endif

	rprint_progress(ofs, size, &now, False);
}

//royc 20100603
void write_progress_file(int value)
{
	if(do_count_progress != 0)
	{
		if(tmp_fp == NULL)
			tmp_fp = fopen(tmp_file_name, "w+");
		
		fseek(tmp_fp, 0, SEEK_SET);
		fprintf(tmp_fp, "%d\n", value);
		
		if(value == 100 || value == -1)
			fclose(tmp_fp);
	}
}

void init_progress_count(void)
{
	//int64 one_pa_size = 0;
	int64 interval_size = 0;
	
	//check flag
	if(table_flag !=0 || do_count_progress == 0)
		return;
		
	//check total size, total size too small
	if(stats.total_size <= MIN_TOTAL_SIZE)
	{
		do_count_progress = 1;
		table_flag = 1;
		write_progress_file(100);
		do_count_progress = 0;
		return;
	}
	
	if (verbose >= 1)
		rprintf(FINFO, "#### total size = %llu\n", stats.total_size);
	
	percent = 0;
	count = 0;
	table_flag = 1;
	interval_size =  (stats.total_size/PROGRESS_NUM);
	//rprintf(FINFO, "#### one_pa_size = %lld\n", one_pa_size);
	
	//we already assign pg_table[0]=0, so we start from 1
	for(count=1; count<=PROGRESS_NUM; count++)
	{
		pg_table[count] = pg_table[count-1] + interval_size;
		
		if (verbose >= 1)
			rprintf(FINFO, "#### pg_table[%d] = %llu\n", count, pg_table[count]);
	}
	count = 0;
	
	//write file to 0 %
	write_progress_file(0);
}

void run_progress_count(int64 judge_value)
{
	int old_percent = 0;
	
	if(do_count_progress != 0 && tmp_fp != NULL && judge_value != 0)
	{
		count ++;
		if(count >= COUNT_MAX_NUM)
		{
			count = 0;
			//rprintf(FINFO, "#### judge_value = %llu\n", judge_value);
			
			old_percent = percent;
			while(judge_value >= pg_table[percent+1])
			{
				percent ++;
			}
			
			if(old_percent != percent)
			{
				if (verbose >= 1)
					rprintf(FINFO, "@@@@ percent = %d\n", (percent<<2));
					
				//write_progress_file((percent*PROGRESS_INTERVAL));
				write_progress_file((percent<<2));
			}
		}
	}
}
