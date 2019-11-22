#1
for i in $(seq 1 5);
do
   ./train 1 model_init.txt data/train_seq_0$i.txt model_0$i.txt 
    #echo $i;
done
./test modellist.txt data/test_seq.txt result.txt
echo "iter = 1:"
python3 check.py
#10
for i in $(seq 1 5);
do
   ./train 10 model_init.txt data/train_seq_0$i.txt model_0$i.txt 
    #echo $i;
done
./test modellist.txt data/test_seq.txt result.txt
echo "iter = 10:"
python3 check.py
#50
for i in $(seq 1 5);
do
   ./train 50 model_init.txt data/train_seq_0$i.txt model_0$i.txt 
    #echo $i;
done
./test modellist.txt data/test_seq.txt result.txt
echo "iter = 50:"
python3 check.py
#100
for i in $(seq 1 5);
do
   ./train 100 model_init.txt data/train_seq_0$i.txt model_0$i.txt 
    #echo $i;
done
./test modellist.txt data/test_seq.txt result.txt
echo "iter = 100:"
python3 check.py
#125
for i in $(seq 1 5);
do
   ./train 125 model_init.txt data/train_seq_0$i.txt model_0$i.txt 
    #echo $i;
done
./test modellist.txt data/test_seq.txt result.txt
echo "iter = 125:"
python3 check.py
