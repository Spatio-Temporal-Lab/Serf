#ifndef ALP_COMPRESSOR_HPP
#define ALP_COMPRESSOR_HPP

#include "alp/encode.hpp"
#include "alp/rd.hpp"
#include "alp/state.hpp"
#include "alp/storer.hpp"
#include "alp/utils.hpp"
#include "fastlanes/ffor.hpp"

namespace alp {

/*
 * API Compressor
 */
struct AlpCompressor {

	state             stt;
	storer::MemStorer storer;

	double   input_vector[config::VECTOR_SIZE];
	double   exceptions[config::VECTOR_SIZE];
	double   sample_array[config::VECTOR_SIZE];
	int64_t  encoded_integers[config::VECTOR_SIZE];
	int64_t  alp_encoded_array[config::VECTOR_SIZE];
	uint16_t exceptions_rd[config::VECTOR_SIZE];
	uint16_t exceptions_position[config::VECTOR_SIZE];

	// 'right' & 'left' refer to the respective parts of the floating numbers after splitting (alprd)
	uint64_t alp_bp_size;
	uint64_t left_bp_size;
	uint64_t right_bp_size;
	uint64_t right_parts[config::VECTOR_SIZE];
	uint64_t right_parts_encoded[config::VECTOR_SIZE];
	uint16_t left_parts_encoded[config::VECTOR_SIZE];
	uint16_t left_parts[config::VECTOR_SIZE];

	AlpCompressor() {}

	size_t get_size() { return storer.get_size(); }

	/*
	 * ALP Compression
	 * Note that Kernels of ALP and FFOR are not fused
	 */
	void compress_vector() {
		if (stt.scheme == SCHEME::ALP_RD) {
			compress_rd_vector();
		} else {
			compress_alp_vector();
		}
	}

	void compress_alp_vector() {
		encode(input_vector, exceptions, exceptions_position, &stt.exceptions_count, encoded_integers, stt);
		analyze_ffor(encoded_integers, stt.bit_width, &stt.for_base);
		ffor::ffor(encoded_integers, alp_encoded_array, stt.bit_width, &stt.for_base);
		alp_bp_size = AlpApiUtils::get_size_after_bitpacking(stt.bit_width);
	}

	void compress_rd_vector() {
		rd_encode(
		    input_vector, exceptions_rd, exceptions_position, &stt.exceptions_count, right_parts, left_parts, stt);
		ffor::ffor(right_parts, right_parts_encoded, stt.right_bit_width, &stt.right_for_base);
		ffor::ffor(left_parts, left_parts_encoded, stt.left_bit_width, &stt.left_for_base);
	}

	void compress(double* values, size_t values_count, uint8_t* out) {
		storer                  = storer::MemStorer(out);
		size_t rouwgroup_count  = AlpApiUtils::get_rowgroup_count(values_count);
		size_t current_idx      = 0;
		size_t left_to_compress = values_count;
		for (size_t current_rowgroup = 0; current_rowgroup < rouwgroup_count; current_rowgroup++) {
			/*
			 * Rowgroup level
			 */
			init(values, current_idx, values_count, sample_array, stt);
			if (stt.scheme == SCHEME::ALP_RD) {
				rd_init(values, current_idx, values_count, sample_array, stt);
				left_bp_size  = AlpApiUtils::get_size_after_bitpacking(stt.left_bit_width);
				right_bp_size = AlpApiUtils::get_size_after_bitpacking(stt.right_bit_width);
			}
			store_rowgroup_metadata();

			size_t values_left_in_rowgroup = std::min(config::ROWGROUP_SIZE, left_to_compress);
			size_t vectors_in_rowgroup     = AlpApiUtils::get_complete_vector_count(values_left_in_rowgroup);
			for (size_t vector_idx = 0; vector_idx < vectors_in_rowgroup; vector_idx++) {
				/*
				 * Vector level
				 */
				for (double& idx : input_vector) {
					idx = values[current_idx++];
				}
				compress_vector();
				store_vector();
				left_to_compress -= config::VECTOR_SIZE;
			}
		}
		if (left_to_compress) { // Last vector which may be incomplete
			stt.vector_size = left_to_compress;
			for (size_t idx = 0; idx < left_to_compress; idx++) {
				input_vector[idx] = values[current_idx++];
			}
			if (stt.scheme == SCHEME::ALP_RD) {
				AlpApiUtils::fill_incomplete_alprd_vector(input_vector, stt);
			} else {
				AlpApiUtils::fill_incomplete_alp_vector(
				    input_vector, exceptions, exceptions_position, &stt.exceptions_count, encoded_integers, stt);
			}
			compress_vector();
			store_vector();
		}
	};

	void compress_rd(double* values, size_t values_count, uint8_t* out) {
		storer                  = storer::MemStorer(out);
		size_t rouwgroup_count  = AlpApiUtils::get_rowgroup_count(values_count);
		size_t current_idx      = 0;
		size_t left_to_compress = values_count;
		for (size_t current_rowgroup = 0; current_rowgroup < rouwgroup_count; current_rowgroup++) {
			/*
			 * Rowgroup level
			 */
			rd_init(values, current_idx, values_count, sample_array, stt);
			left_bp_size  = AlpApiUtils::get_size_after_bitpacking(stt.left_bit_width);
			right_bp_size = AlpApiUtils::get_size_after_bitpacking(stt.right_bit_width);

			store_rowgroup_metadata();

			size_t values_left_in_rowgroup = std::min(config::ROWGROUP_SIZE, left_to_compress);
			size_t vectors_in_rowgroup     = AlpApiUtils::get_complete_vector_count(values_left_in_rowgroup);
			for (size_t vector_idx = 0; vector_idx < vectors_in_rowgroup; vector_idx++) {
				/*
				 * Vector level
				 */
				for (size_t idx = 0; idx < config::VECTOR_SIZE; idx++) {
					input_vector[idx] = values[current_idx++];
				}
				compress_rd_vector();
				store_rd_vector();
				left_to_compress -= config::VECTOR_SIZE;
			}
		}
		if (left_to_compress) { // Last vector which may be incomplete
			stt.vector_size = left_to_compress;
			for (size_t idx = 0; idx < left_to_compress; idx++) {
				input_vector[idx] = values[current_idx++];
			}
			AlpApiUtils::fill_incomplete_alprd_vector(input_vector, stt);
			compress_rd_vector();
			store_rd_vector();
		}
	}

	void store_rd_vector() {
		storer.store((void*)&stt.exceptions_count, sizeof(stt.exceptions_count));
		storer.store((void*)left_parts_encoded, left_bp_size);
		storer.store((void*)right_parts_encoded, right_bp_size);
		if (stt.exceptions_count) {
			storer.store((void*)exceptions_rd, rd_exception_size_bytes * stt.exceptions_count);
			storer.store((void*)exceptions_position, rd_exception_position_size_bytes * stt.exceptions_count);
		}
	}

	void store_alp_vector() {
		storer.store((void*)&stt.exp, sizeof(stt.exp));
		storer.store((void*)&stt.fac, sizeof(stt.fac));
		storer.store((void*)&stt.exceptions_count, sizeof(stt.exceptions_count));
		storer.store((void*)&stt.for_base, sizeof(stt.for_base));
		storer.store((void*)&stt.bit_width, sizeof(stt.bit_width));
		storer.store((void*)alp_encoded_array, alp_bp_size);
		if (stt.exceptions_count) {
			storer.store((void*)exceptions, exception_size_bytes * stt.exceptions_count);
			storer.store((void*)exceptions_position, exception_position_size_bytes * stt.exceptions_count);
		}
	}

	void store_schema() {
		uint8_t scheme_code = (uint8_t)stt.scheme;
		storer.store((void*)&scheme_code, sizeof(scheme_code));
	}

	void store_vector() {
		if (stt.scheme == SCHEME::ALP_RD) {
			store_rd_vector();
		} else {
			store_alp_vector();
		}
	}

	void store_rd_metadata() {
		storer.store((void*)&stt.right_bit_width, sizeof(stt.right_bit_width));
		storer.store((void*)&stt.left_bit_width, sizeof(stt.left_bit_width));
		storer.store((void*)&stt.actual_dictionary_size, sizeof(stt.actual_dictionary_size));
		storer.store((void*)stt.left_parts_dict, stt.actual_dictionary_size_bytes);
	}

	void store_rowgroup_metadata() {
		store_schema();
		if (stt.scheme == SCHEME::ALP_RD) { store_rd_metadata(); }
	}
};

} // namespace alp

#endif