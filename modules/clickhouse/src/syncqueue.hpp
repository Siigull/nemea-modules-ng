/**
 * @file
 * @author Michal Sedlak <sedlakm@cesnet.cz>
 * @brief SyncQueue class implementation
 * @date 2024
 *
 * Copyright(c) 2024 CESNET z.s.p.o.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

/**
 * @brief A thread-safe queue
 */
template <typename Item>
class SyncQueue {
public:
	/**
	 * @brief Put an item into the queue
	 *
	 * @param item The item
	 */
	void put(Item item)
	{
		std::lock_guard<std::mutex> const lock(m_mutex);
		m_items.push(item);
		m_size = m_items.size();
		m_avail_cv.notify_all();
	}

	/**
	 * @brief Get an item from the queue, block and wait if there aren't any
	 *
	 * @return The item
	 */
	Item get()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		while (true) {
			if (!m_items.empty()) {
				auto item = m_items.front();
				m_items.pop();
				m_size = m_items.size();
				return item;
			}
			m_avail_cv.wait(lock);
		}
	}

	/**
	 * @brief Get the current size of the queue
	 *
	 * @return The number of items in the queue
	 */
	std::size_t size() { return m_size; }

private:
	std::atomic_size_t m_size = 0; ///< current number of items in the queue
	std::queue<Item> m_items; ///< underlying container for queued items
	std::mutex m_mutex; ///< mutex for synchronizing access
	std::condition_variable m_avail_cv; ///< signals availability of items
};
