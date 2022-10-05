#ifndef MEDIANFILTER_H
#define MEDIANFILTER_H

#define USER_NULL 0
#define STOPPER 0
#define MEDIAN_FILTER_SIZE 11
#define WINDOWS_SIZE 31

template <typename T>
class MedianFilter
{

public:
	  MedianFilter(int s);
	  //~MedianFilter();
	  T calculation(T);
	  T getBiggest() const {return big.point->value;}

private:
	struct pair{
	  struct pair *point; /* Pointers forming list linked in sorted order */
	  T value; /* Values to sort */
	};

	  //static int MEDIAN_FILTER_SIZE;
	 /* Buffer of nwidth pairs */
	  //struct pair *buffer;        // динамический массив
	  //buffer = new struct pair [asd];
//TODO в таком виде не работает, нужно static 4 первых переменных,тогда ошибка компиляции
	 /*Size filtr*/
	  int size;
	 /* Buffer of nwidth pairs */
	  //struct pair buffer[MEDIAN_FILTER_SIZE];// = {0};!!!
	  struct pair *buffer;
	 /* Pointer into circular buffer of data */
	  struct pair *datpoint;// = buffer;!!!
	 /* Chain stopper */
	  struct pair small;// = {USER_NULL, STOPPER};!!!
	 /* Pointer to head (largest) of linked list.*/
	  struct pair big;// = {&small, 0};!!!

	 /* Pointer to successor of replaced data item */
	  struct pair *successor;
	 /* Pointer used to scan down the sorted list */
	  struct pair *scan;
	 /* Previous value of scan */
	  struct pair *scanold;
	 /* Pointer to median */
	  struct pair *median;

};

template <typename T>
MedianFilter<T>::MedianFilter(int s)
{
	switch(s)
	{
	   case 3:
	   case 5:
	   case 7:
	   case 9:
	   case 11:
	   case 13:
	   case 15:
		   size = s;
	   break;
	   default:
		   size = 5;
	   break;
	}
	buffer = new pair[size];
	datpoint = buffer;
	small = {USER_NULL, STOPPER};
	big = {&small, 0};
	big = {&small, 0};
}

template <typename T>
T MedianFilter<T>::calculation(T datum)
{
	int i;

	if (datum == STOPPER){
	   datum = STOPPER + 1; /* No stoppers allowed. */
	}

	 if ( (++datpoint - buffer) >= MEDIAN_FILTER_SIZE){
	   datpoint = buffer; /* Increment and wrap data in pointer.*/
	 }

	 datpoint->value = datum; /* Copy in new datum */
	 successor = datpoint->point; /* Save pointer to old value's successor */
	 median = &big; /* Median initially to first in chain */
	 scanold = USER_NULL; /* Scanold initially null. */
	 scan = &big; /* Points to pointer to first (largest) datum in chain */

	 /* Handle chain-out of first item in chain as special case */
	 if (scan->point == datpoint){
	   scan->point = successor;
	 }

	 scanold = scan; /* Save this pointer and */
	 scan = scan->point ; /* step down chain */

	 /* Loop through the chain, normal loop exit via break. */
	 for (i = 0 ; i < MEDIAN_FILTER_SIZE; ++i){
	  /* Handle odd-numbered item in chain */
	  if (scan->point == datpoint){
		scan->point = successor; /* Chain out the old datum.*/
	  }

	  if (scan->value < datum){ /* If datum is larger than scanned value,*/
		datpoint->point = scanold->point; /* Chain it in here. */
		scanold->point = datpoint; /* Mark it chained in. */
		datum = STOPPER;
	  };

	  /* Step median pointer down chain after doing odd-numbered element */
	  median = median->point; /* Step median pointer. */
	  if (scan == &small){
		break; /* Break at end of chain */
	  }
	  scanold = scan; /* Save this pointer and */
	  scan = scan->point; /* step down chain */

	  /* Handle even-numbered item in chain. */
	  if (scan->point == datpoint){
		scan->point = successor;
	  }

	  if (scan->value < datum){
		datpoint->point = scanold->point;
		scanold->point = datpoint;
		datum = STOPPER;
	  }

	  if (scan == &small){
		break;
	  }

	  scanold = scan;
	  scan = scan->point;
	}

	return median->value;
}

#endif // MEDIANFILTER_H
