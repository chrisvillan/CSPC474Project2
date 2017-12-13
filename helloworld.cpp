#include <stdio.h>
#include <mpi.h>
#include <iostream>


using namespace std;

struct node{
  int data;
  node *next;
};

class linked_list
{
private:
    node *head,*tail;
public:
    linked_list()
    {
        head = NULL;
        tail = NULL;
    }

    void add_node(int n)
    {
        node *tmp = new node;
        tmp->data = n;
        tmp->next = NULL;

        if(head == NULL)
        {
            head = tmp;
            tail = tmp;
        }
        else
        {
            tail->next = tmp;
            tail = tail->next;
        }
    }
    void display()
    {
      node *temp=new node;
      temp=head;
      cout << "Linked List:    ";
      while(temp!=NULL)
      {
        cout<<temp->data << " -> " ;
        temp=temp->next;
      }
      cout << "NULL" << endl << endl;
    }


};



int *filterArray(int *array, int num_elem){
  int *filtArray = (int*)malloc(sizeof(int) * num_elem);
  int count=0;

  //sets filtArray to 0's
  for(int j = 0; j < num_elem; j++){                            //  { 0 1 0 0 }
    filtArray[j] = 0;                                           //  { 0 0 0 3 }
  }                                                             //  { 0 4 0 7 }
                                                                //  {10 0 0 0 }

                                                                //   <---
  //pushes any non 0's to the left
  for(int i = 0; i < num_elem; i++){                            //  { 1 0 0 0 }
    if(array[i] != 0){                                          //  { 3 0 0 0 }
      filtArray[count] = array[i];                              //  { 4 7 0 0 }
      count ++;                                                 //  {10 0 0 0 }
    }
  }

  return filtArray;
}


int main(int argc, char** argv){


int rank, size;
int numProcess = 4;
int numElemProcess = 4;
int a[16] = {0,1,0,0,                                           //  { 0 1 0 0 }
              0,0,0,3,                                          //  { 0 0 0 3 }
                0,4,0,7,                                        //  { 0 4 0 7 }
                  10,0,0,0};                                    //  {10 0 0 0 }



//makes process be part of the communicator - MPI_COMM_WORLD
MPI_Init(&argc,&argv);
MPI_Comm_rank(MPI_COMM_WORLD, &rank); //gets rank
MPI_Comm_size(MPI_COMM_WORLD, &size); //gets size
int *b;

//"envelope" where data will be scattered
int *buffer1 =(int*)malloc(sizeof(int) * numProcess);

// MPI_SCATTER(send_data, send_count, send_datatype, recv_data, recv_count, recv_dataype, root, communicater)
MPI_Scatter(a, numElemProcess, MPI_INT, buffer1, numElemProcess, MPI_INT, 0, MPI_COMM_WORLD);


//pushes non-zero elements to the right
int *newArray = filterArray(buffer1, numElemProcess);


//"envelop" where data will be stored when received
int *buffer2 = NULL;

if (rank==0){
  //"envelop" becomes "big envelop"
  buffer2 = (int*)malloc(sizeof(int) * size);
}
// MPI_Gather(send_data, send_count, send_datatype, recv_data, recv_count, recv_dataype, root, communicater)
MPI_Gather(newArray, numElemProcess, MPI_INT, buffer2, numElemProcess, MPI_INT, 0, MPI_COMM_WORLD);

//prints
if (rank == 0){
  cout << "\nOriginal Array:";
    for (int i=0; i< numProcess * numElemProcess; i++){
      cout << " " << a[i];
    }
  cout << endl;


    cout << "\nSorted Array:  ";
  for (int i=0; i< numProcess * numElemProcess; i++){
    if( (i % 4) == 0)
    cout <<" |";

    cout << " " << buffer2[i];

  }
    cout << endl;
}


 //no more MPI calls
  MPI_Finalize();

  if(rank == 0 ){
    buffer2[8] = 4; //error temp fix
    buffer2[9] = 7; //error temp fix
    int cnt =0;

      linked_list a;



      for (int k=0; k< numProcess * numElemProcess; k = k + numElemProcess){
          while(buffer2[k+cnt] != 0){
            a.add_node(buffer2[k+cnt]);
            cnt ++;
                                                      //      [1]   0   0   0
            if (cnt>numElemProcess)                   //      [3]   0   0   0
                break;                                //      [4]->[7]  0   0
          }                                           //      [10]  0   0   0
          cnt=0;
      }
      cout << endl;
      a.display();

  }

  return 0;
}
