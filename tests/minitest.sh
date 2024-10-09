#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

# Number of requests to send
NUM_REQUESTS=1
TOTAL_SUCCESS=0

# URLs to test
URLS=(
    "http://127.0.0.1:8080"
    "http://127.0.0.2:8080"
    "http://127.0.0.1:8081"
    "http://127.0.0.2:8081"
)


# Run stress test for all URLs in parallel and collect success counts
for url in "${URLS[@]}"; do
{
    for ((i=1; i<=NUM_REQUESTS; i++)); do
    {
        hurl-3.0.0/hurl --variable domain=$url --test test/tests.hurl #&> /dev/null
        if [ $? -ne 0 ]; then
            echo -e "${RED}Error: Request to $url failed${NC}"
        else
            ((TOTAL_SUCCESS++))
        fi
    }
    done
} 
done

# Wait for all background processes to finish
wait

# Calculate the percentage of successful requests
total_requests=$((NUM_REQUESTS * ${#URLS[@]}))
val=$((TOTAL_SUCCESS * 1000 / total_requests))  # Multiply by 1000 for more precision

# Convert to percentage with decimal (divide by 10)
percentage=$(echo "scale=1; $val / 10" | bc)

echo -e "${BLUE}Stress test completed.${NC}"
echo -e "${BLUE}Passed $percentage% of the tests.${NC}"
