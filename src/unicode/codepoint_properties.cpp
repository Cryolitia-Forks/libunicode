/**
 * This file is part of the "libunicode" project
 *   Copyright (c) 2020 Christian Parpart <christian@parpart.family>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <unicode/codepoint_properties.h>
#include <unicode/codepoint_properties_data.h>

namespace unicode
{

codepoint_properties::tables_view codepoint_properties::configured_tables { precompiled::stage1,
                                                                            precompiled::stage2,
                                                                            precompiled::properties };

} // namespace unicode