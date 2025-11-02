public class QuickSort extends SortAlgorithm {

	public QuickSort(int input_array[]) {
		super(input_array);
	}
	
    private int partition(int arr[], int low, int high){
        int pivot = arr[high]; // select the last element as pivot
        int i = (low - 1); // index for element which is smaller or equal to pivot

        for(int j = low; j < high; ++j){
            comparison_counter++; // increment counter for each comparison
            if(arr[j] <= pivot){
                i++;
                swap(i, j); // moves elements less than or equal to the pivot to the left of the pivot.
            }
        }
        swap(i + 1, high); // place the pivot element in the right place.
        return i + 1; // return  new pivot index
    }

    private void sort(int arr[], int low, int high){
        if(low < high){
            int pi = partition(arr, low, high); // the array divded into 2 
            sort(arr, low, pi - 1); // left array is sorting recursively
            sort(arr, pi + 1, high); // right array is sorting recursively
        }
    }

    @Override
    public void sort() {
    	sort(arr, 0, arr.length - 1); // call private sort() method
    }

    @Override
    public void print() {
    	System.out.print("Quick Sort\t=>\t");
    	super.print();
    }
}
